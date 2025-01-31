// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// game.h - Game data structures and implementation
#pragma once
#include "audio.h"
#include "camera.h"
#include "collision.h"
#include "engine.h"
#include "game_audio.h"
#include "game_debug.h"
#include "image.h"
#include "level.h"
#include "mem.h"
#include "monster.h"
#include "music.h"
#include "player.h"
#include "rand.h"
#include "sparse_set.h"
#include "types.h"
#include "vec.h"
#include "wall.h"

/*
Game Design V1.0
- Only one hit-scan gun
- Monsters creep towards you and try to eat you
- Monsters are generated pixel art sprites
- Levels are generated
- World is 2d maze with walls
    - grid aligned
- single player
- walls can be painted
*/

typedef enum {
    DBG_None,
    DBG_Entity,
    DBG_Texture,
    DBG_Collision,
    DBG_COUNT,
} Game_Debug;

typedef struct {
    Memory *mem;
    Entity *player;
    Entity *monsters;
    Image *gun;
    Camera camera;
    u32 debug;

    Sparse_Set *sparse;
    Music music;
} Game;

static Image *gen_gun(Memory *mem, Random *rng) {
    u32 length = 8;
    u32 height = 3;

    u32 size = 8;
    Image *img = image_new(mem, (v2u){length, height + 3});
    // image_grid(img, (v4){1, 0, 0, 1}, (v4){0, 0, 1, 1});

    v3 color_barrel = rand_color(rng) * 0.2;
    v3 color_sight = rand_color(rng) * 0.2;
    v3 color_grip = rand_color(rng) * 0.2;

    // Barrel
    for (u32 x = 0; x < length; ++x) {
        for (u32 y = 0; y < height; ++y) {
            image_write(img, (v2i){x, y + 1}, color_barrel);
        }
    }

    // Sight
    image_write(img, (v2i){1, 0}, color_sight);
    image_write(img, (v2i){length - 1, 0}, color_sight);

    for (u32 x = 0; x < 2; ++x) {
        image_write(img, (v2i){length - 1 - x, height + 1}, color_grip);
        image_write(img, (v2i){length - 1 - x, height + 2}, color_grip);
    }

    img->origin.x = length - 2;
    img->origin.y = img->size.y - 2;
    return img;
}

static Image *gen_cursor(Memory *mem) {
    u32 size = 5;
    Image *img = image_new(mem, (v2u){size, size});
    for (u32 i = 0; i < size; ++i) {
        v4 col = color_alpha(WHITE, 1);
        image_write4(img, (v2i){i, i}, col);
        image_write4(img, (v2i){i, size - 1 - i}, col);
    }
    return img;
}

static void game_gen_monsters(Game *game, Random *rng, v3i spawn) {
    Sprite_Properties s1 = sprite_new(rng);
    Sprite_Properties s2 = sprite_new(rng);

    // Player
    Sprite_Properties s = sprite_new(rng);
    Entity *player = monster_new(game->mem, rng, v3i_to_v3(spawn), s);
    player->is_monster = false;
    player->is_player = true;

    // Insert
    player->next = game->monsters;
    game->monsters = player;
    game->player = player;

    for (Entity *wall = game->monsters; wall; wall = wall->next) {
        // Only consider walls
        if (!wall->is_wall) continue;
        if (wall->mtx.z.y < 0.5) continue;

        // Don't generate them too close
        f32 spawn_area = 4;
        if (v3_distance_sq(wall->pos, player->pos) < spawn_area * spawn_area) continue;

        // Choose random sprite props
        Sprite_Properties prop = s1;
        if (rand_f32(rng) > 0.5) prop = s2;

        Entity *mon = monster_new(game->mem, rng, wall->pos, prop);
        // Insert
        mon->next = game->monsters;
        game->monsters = mon;
    }
}

// Create a new game
static Game *game_new(Random *rng) {
    v2i level_size = {8, 8};
    v2i spawn = level_size / 2;

    Memory *mem = mem_new();
    Game *game = mem_struct(mem, Game);
    game->mem = mem;

    // Create Level
    level_generate(&game->monsters, mem, rng, level_size);

    // Generate player
    game->gun = gen_gun(mem, rng);

    // Generate Monsters
    game_gen_monsters(game, rng, (v3i){spawn.x, 0, spawn.y});
    game->camera.target = game->player;

    game->sparse = sparse_set_new(mem);
    music_init(&game->music);
    return game;
}

static void player_apply_input(Engine *eng, Entity *ent, Player_Input *in) {
    // Update player head rotation
    // x -> pitch
    // y -> yaw
    // z -> roll
    ent->rot.xy += in->look.xy;

    // Limit pitch to full up and full down
    ent->rot.x = f_clamp(ent->rot.x, -0.5 * PI, 0.5 * PI);

    // wraparound yaw
    ent->rot.y = f_wrap(ent->rot.y, -PI, PI);

    // Ease towards target Roll
    ent->rot.z += (in->look.z - ent->rot.z) * 10 * eng->dt * PI;

    // Jumping
    if (in->jump && ent->on_ground && ent->health > 0) {
        ent->pos_old.y = ent->pos.y;
        ent->pos.y += 4 * eng->dt;
        game_audio_jump(eng);
    }

    // Flying
    if (in->fly) ent->is_flying = !ent->is_flying;

    // Apply movement input
    m4 yaw_mtx = m4_id();
    m4_rotate_y(&yaw_mtx, ent->rot.y); // Yaw

    v3 move = m4_mul_dir(yaw_mtx, in->move);
    move.xz = v2_limit(move.xz, 0, 1);
    move.y = in->move.y * ent->is_flying;
    ent->pos += move * 2.0 * eng->dt;
}

typedef struct {
    bool hit;
    v3 pos;
    v2 uv;
    v2 pixel;
    f32 distance;
} Collide_Result;

static Collide_Result collide_quad_ray(m4 quad_mtx, Image *img, v3 ray_pos, v3 ray_dir) {
    Collide_Result result = {};

    Quad quad = {quad_mtx, v2u_to_v2(img->size) * 0.5f * GFX_PIXEL_SCALE_3D};

    // Matrix inverse, can calculate points from Global to Local
    m4 mtx_inv = m4_invert_tr(quad.mtx);

    // Compute Ray position and direction in quad local space
    v3 ray_pos_local = m4_mul_pos(mtx_inv, ray_pos);
    v3 ray_dir_local = m4_mul_dir(mtx_inv, ray_dir);

    // Total distance to the plane along the ray
    f32 distance = ray_pos_local.z / -ray_dir_local.z;

    // Ray moves away from plane
    if (distance < 0) return result;

    // Compute ray hit position
    v3 hit_local = ray_pos_local + ray_dir_local * distance;
    v3 hit_global = ray_pos + ray_dir * distance;

    if (hit_local.x > 0.5) return result;
    if (hit_local.x < -0.5) return result;
    if (hit_local.y > 0.5) return result;
    if (hit_local.y < -0.5) return result;

    v2 pixel = hit_local.xy;
    pixel.y *= -1;
    pixel += (v2){0.5f, 0.5f};
    pixel *= v2u_to_v2(img->size);

    v4 *px = image_get(img, (v2i){pixel.x, pixel.y});
    if (!px) return result;
    if (px->w <= 0.1) return result;

    return (Collide_Result){
        .hit = true,
        .pos = hit_global,
        .uv = hit_local.xy,
        .pixel = pixel,
        .distance = distance,
    };
}

// Player update function
static void player_update(Entity *pl, Game *game, Engine *eng) {
    Player_Input in = {};
    Camera *camera = &game->camera;
    if (camera->target == pl) {
        in = player_parse_input(eng->input);
    }
    entity_update_movement(pl, eng);
    player_apply_input(eng, pl, &in);

    entity_collide(eng, game->sparse, pl);
    if (camera->target == pl) {
        camera_bob(camera, v2_length(pl->vel.xz));
    }

    // Update matricies
    {
        pl->mtx = m4_id();
        m4_rotate_y(&pl->mtx, pl->rot.y);
        m4_translate(&pl->mtx, pl->pos);

        pl->head_mtx = m4_id();
        m4_rotate_z(&pl->head_mtx, pl->rot.z);
        m4_rotate_x(&pl->head_mtx, pl->rot.x);
        m4_rotate_y(&pl->head_mtx, pl->rot.y);
        m4_translate(&pl->head_mtx, pl->pos);
        m4_translate_y(&pl->head_mtx, .5);
    }

    // Shooting
    pl->recoil_animation = animate(pl->recoil_animation, -eng->dt * 4);
    if (in.shoot && pl->recoil_animation == 0) {
        pl->recoil_animation = 1;
        camera_shake(&game->camera, 0.5);
        game_audio_shoot(eng);

        v3 ray_pos = pl->head_mtx.w;
        v3 ray_dir = pl->head_mtx.z;

        Collide_Result best_result = {0};
        Entity *best_monster = 0;
        for (Entity *mon = game->monsters; mon; mon = mon->next) {
            if (mon == pl) continue;
            Collide_Result result = collide_quad_ray(mon->image_mtx, mon->image, ray_pos, ray_dir);
            if (!result.hit) continue;
            if (best_result.hit && result.distance > best_result.distance) continue;
            best_monster = mon;
            best_result = result;
        }

        if (best_monster) {
            Image *img = best_monster->image;

            // Damage entity
            if (best_monster->health > 0) {
                best_monster->health--;

                // Entity just died
                if (best_monster->health == 0) {
                    for (u32 y = 0; y < img->size.y; ++y) {
                        for (u32 x = 0; x < img->size.x; ++x) {
                            v4 *px = img->pixels + y * img->size.x + x;
                            px->xyz = BLEND(px->xyz, GRAY, 0.5f);
                        }
                    }
                }
            }

            for (u32 i = 0; i < 32; ++i) {
                f32 ox = rand_f32_signed(&eng->rng);
                f32 oy = rand_f32_signed(&eng->rng);

                ox = ox * ox - 0.5;
                oy = oy * oy - 0.5;

                i32 x = best_result.pixel.x + ox * best_result.distance * 4;
                i32 y = best_result.pixel.y + oy * best_result.distance * 4;
                v4 *px = image_get(img, (v2i){x, y});
                if (!px) continue;

                px->xyz = BLEND(px->xyz, best_monster->sprite.blood_color, 0.6);
            }

            img->variation++;
        }
    }

    // Draw Gun
    {
        m4 mtx = m4_id();
        m4_image_3d(&mtx, game->gun);
        m4_rotate_y(&mtx, R1);
        m4_rotate_x(&mtx, BLEND(0, -R1 * .2, pl->recoil_animation));
        m4_translate_x(&mtx, -0.2);
        m4_translate_y(&mtx, -0.15);
        m4_translate_z(&mtx, BLEND(0.3, 0.1, pl->recoil_animation));
        m4_apply(&mtx, pl->head_mtx);
        gfx_quad_3d(eng->gfx, mtx, game->gun);
    }

    if (pl->shadow) draw_shadow(eng, pl->mtx.w, pl->shadow);
    if (pl->image) {
        gfx_quad_3d(eng->gfx, pl->mtx, pl->image);
    }
    // gfx_draw_mtx(eng, pl->head_mtx);
}

static void wall_update(Game *game, Engine *eng, Entity *ent) {
    v2 size = v2u_to_v2(ent->image->size) / 32.0f;
    m4 mtx = m4_id();
    m4_image_3d(&mtx, ent->image);
    m4_apply(&mtx, ent->mtx);
    gfx_quad_3d(eng->gfx, mtx, ent->image);
    ent->image_mtx = mtx;

    Box box = box_from_quad((Quad){ent->mtx, size * .5});
    sparse_set_add(game->sparse, box, ent);
}

static void entity_update(Engine *eng, Game *game, Entity *ent) {
    if (ent->is_monster) monster_update(ent, game->player, game->gun, game->sparse, eng);
    if (ent->is_player) player_update(ent, game, eng);
    if (ent->is_wall) wall_update(game, eng, ent);
    if (game->debug == DBG_Entity) gfx_debug_mtx(eng->gfx_dbg, ent->image_mtx);
}

static void game_update(Game *game, Engine *eng) {
    Player_Input input = player_parse_input(eng->input);

    // Debug draw sparse data
    if (game->debug == DBG_Collision) {
        sparse_debug_draw(eng, game->sparse, game->player);
    }

    // Toggle freecam
    if (key_click(eng->input, KEY_3)) {
        camera_follow(&game->camera, game->camera.target ? 0 : game->player);
    }

    // Toggle debug drawing
    if (key_click(eng->input, KEY_4)) {
        game->debug = (game->debug + 1) % DBG_COUNT;
    }

    camera_input(&game->camera, &input, eng->dt);

    for (Entity *ent = game->monsters; ent; ent = ent->next) {
        entity_update(eng, game, ent);
    }

    camera_update(&game->camera, eng->dt);
    // music_play(&game->music, eng);

    // Update bvh
    sparse_set_swap(game->sparse);
}

static void game_free(Game *game) {
    mem_free(game->mem);
}
