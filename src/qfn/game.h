// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// game.h - Game data structures and implementation
#pragma once
#include "lib/mem.h"
#include "lib/rand.h"
#include "lib/types.h"
#include "lib/vec.h"
#include "qfn/audio.h"
#include "qfn/camera.h"
#include "qfn/collision.h"
#include "qfn/engine.h"
#include "qfn/game_debug.h"
#include "qfn/gun.h"
#include "qfn/image.h"
#include "qfn/level.h"
#include "qfn/monster.h"
#include "qfn/monster2.h"
#include "qfn/player.h"
#include "qfn/sparse_set.h"

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

typedef struct {
    Memory *mem;
    Entity *player;
    Entity *monsters;

    Player *player2;
    Monster *monster2_list;

    Image *gun;
    Camera camera;
    Game_Debug debug;
    Sparse_Set *sparse;
    Audio audio;
} Game;

static void game_gen_monsters(Game *game, Rand *rng, v3i spawn) {
    Sprite_Properties s1 = sprite_new(rng);
    Sprite_Properties s2 = sprite_new(rng);

    // Player
    Sprite_Properties s = sprite_new(rng);
    Entity *player = monster_new(game->mem, rng, v3i_to_v3(spawn), s);
    player->type = Entity_Player;

    // Insert
    player->next = game->monsters;
    game->monsters = player;
    game->player = player;

    game->player2 = player2_new(game->mem, v3i_to_v3(spawn), game->gun);

    for (Entity *wall = game->monsters; wall; wall = wall->next) {
        // Only consider walls
        if (wall->type != Entity_Wall) continue;
        if (wall->mtx.z.y < 0.5) continue;

        // Don't generate them too close
        f32 spawn_area = 4;
        if (v3_distance_sq(wall->pos, player->pos) < spawn_area * spawn_area) continue;

        // Choose random sprite props
        Sprite_Properties prop = s1;
        if (rand_choice(rng, 0.5)) prop = s2;

        Monster *mon = monster2_new(game->mem, wall->pos, prop);
        mon->next = game->monster2_list;
        mon->gun = game->gun;
        game->monster2_list = mon;
    }
}

// Create a new game
static Game *game_new(Rand *rng) {
    v2i level_size = {8, 8};
    v2i spawn = 0;

    Memory *mem = mem_new();
    Game *game = mem_struct(mem, Game);
    game->mem = mem;

    // Create Level
    level_generate(&game->monsters, mem, rng, level_size);

    // Generate player
    game->gun = gun_new(mem, rng);

    // Generate Monsters
    game_gen_monsters(game, rng, (v3i){spawn.x, 0, spawn.y});
    // game->camera.target = game->player2;

    game->sparse = sparse_set_new(mem);
    game->audio.snd = sound_init(mem);

    Image *img = image_new(mem, (v2u){32, 32});
    image_fill(img, (v4){1, 0, 1, 1});
    return game;
}

static void player_apply_input(Game *game, Engine *eng, Entity *ent, Player_Input *in) {
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
        audio_jump(&game->audio);
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
    player_apply_input(game, eng, pl, &in);

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
        audio_shoot(&game->audio);

        for (u32 i = 0; i < 16; ++i) {
            v3 ray_pos = pl->head_mtx.w;
            v3 ray_dir = pl->head_mtx.z;

            f32 a = rand_f32(&eng->rng, -1, 1);
            f32 r = rand_f32(&eng->rng, -1, 1);
            f32 ox = f_cos2pi(a) * r * 0.1;
            f32 oy = f_sin2pi(a) * r * 0.1;

            ray_dir += pl->head_mtx.x * ox;
            ray_dir += pl->head_mtx.y * oy;

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

                i32 x = best_result.pixel.x;
                i32 y = best_result.pixel.y;
                v4 *px = image_get(img, (v2i){x, y});
                if (!px) continue;

                px->xyz = BLEND(px->xyz, best_monster->sprite.blood_color, 0.6);

                img->variation++;
            }
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
    if (ent->type == Entity_Monster) monster_update(ent, game->player, game->gun, game->sparse, eng);
    if (ent->type == Entity_Player) player_update(ent, game, eng);
    if (ent->type == Entity_Wall) wall_update(game, eng, ent);
    if (game->debug == DBG_Entity) debug_draw_entity(eng, ent);
}

static void game_update(Game *game, Engine *eng) {
    Player_Input input = player_parse_input(eng->input);

    // Debug draw sparse data
    if (game->debug == DBG_Collision) {
        debug_draw_collisions(eng, game->sparse, game->player);
    }

    // Toggle freecam
    if (key_click(eng->input, KEY_3)) {
        camera_follow(&game->camera, game->camera.target ? 0 : game->player);
    }

    // Toggle debug drawing
    if (key_click(eng->input, KEY_4)) {
        debug_next(&game->debug);
    }

    camera_input(&game->camera, &input, eng->dt);

    for (Entity *ent = game->monsters; ent; ent = ent->next) {
        entity_update(eng, game, ent);
    }

    player2_update(game->player2, eng);
    for (Monster *mon = game->monster2_list; mon; mon = mon->next) {
        monster2_update(mon, eng, &game->audio, game->sparse, game->player->pos);
    }

    camera_update(&game->camera, eng->dt);

    // Update bvh
    sparse_set_swap(game->sparse);
}

static void game_free(Game *game) {
    mem_free(game->mem);
}
