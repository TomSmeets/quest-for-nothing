// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// game.h - Game data structures and implementation
#pragma once
#include "audio.h"
#include "camera.h"
#include "engine.h"
#include "game_audio.h"
#include "image.h"
#include "level.h"
#include "mem.h"
#include "monster.h"
#include "player.h"
#include "rand.h"
#include "sparse_set.h"
#include "types.h"
#include "vec.h"

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

#define GFX_PIXEL_SCALE_UI 4.0f
#define GFX_PIXEL_SCALE_3D (1.0f / 32.0f)

static void m4_image_at_scale(m4 *mtx, Image *img, f32 scale) {
    v2 size = {img->size.x, img->size.y};
    v2 origin = {img->origin.x, img->origin.y};
    size *= scale;
    origin *= scale;

    // Scale to image size, 1 unit = 1 pixel
    m4_scale(mtx, (v3){size.x, size.y, 1});

    // Center at origin
    m4_translate_x(mtx, 0.5 * size.x - origin.x);
    m4_translate_y(mtx, origin.y - 0.5 * size.y);
}

static void m4_image_3d(m4 *mtx, Image *img) {
    m4_image_at_scale(mtx, img, GFX_PIXEL_SCALE_3D);
}

static void m4_image_ui(m4 *mtx, Image *img) {
    m4_image_at_scale(mtx, img, GFX_PIXEL_SCALE_UI);
}

#define SHADOW_OFFSET 0.01
#define MONSTER_OFFSET 0.02

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
    Monster *monsters;
    Image *gun;
    Camera camera;
    u32 debug;

    Sparse_Set *sparse;
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
    Monster *player = monster_new(game->mem, rng, v3i_to_v3(spawn), s);
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

        Monster *mon = monster_new(game->mem, rng, wall->pos, prop);
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
    return game;
}

static f32 animate(f32 x, f32 dt) {
    x += dt;
    return f_clamp(x, 0, 1);
}

static bool animate2(f32 *x, f32 dt) {
    *x -= dt;
    return *x <= 0;
}

static void animate_exp(f32 *value, f32 target, f32 dt) {
    *value += (target - *value) * dt;
}

static bool animate_lin(f32 *value, f32 target, f32 dt) {
    if (*value > target + dt) {
        *value -= dt;
        return false;
    } else if (*value < target - dt) {
        *value += dt;
        return false;
    } else {
        *value = target;
        return true;
    }
}

static void monster_update_eyes(Entity *mon, Engine *eng) {
    if (animate2(&mon->look_around_timer, eng->dt)) {
        mon->look_around_timer = rand_f32_range(&eng->rng, 1, 8);
        monster_sprite_update_eyes(&mon->sprite, &eng->rng);
    }
}

static void monster_update_ai(Entity *mon, Game *game, Engine *eng) {
    // Apply movement
    mon->pos.xz += mon->move_dir.xy * eng->dt;

    // AI movement
    if (animate2(&mon->move_time, eng->dt)) {
        mon->move_time = rand_f32_range(&eng->rng, 2, 10);

        // AI Movement mode
        u32 mode = rand_u32_range(&eng->rng, 0, 2);

        // Stand still
        if (mode == 0) mon->move_dir = 0;

        // To player
        if (mode == 1) mon->move_dir = v2_normalize(game->player->pos.xz - mon->pos.xz) * 0.1;

        // Random direction
        if (mode == 2) mon->move_dir = v2_from_rot(rand_f32_signed(&eng->rng) * PI) * 0.25;
    }

    // Look direction
    mon->look_dir = v3_normalize((game->player->pos - mon->pos) * (v3){1, 0, 1});
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

static void entity_update_movement(Monster *mon, Engine *eng) {
    bool do_gravity = !mon->is_flying;
    bool do_ground_collision = !mon->is_flying;

    // Reset state
    mon->on_ground = false;

    // Physics
    v3 vel_dt = mon->pos - mon->pos_old;
    mon->vel = vel_dt / eng->dt;
    mon->pos_old = mon->pos;

    if (do_gravity) {
        mon->pos.y -= 9.81 * eng->dt * eng->dt;
        mon->pos.y += vel_dt.y;
    }

    if (do_ground_collision && mon->pos.y < 0) {
        mon->pos.y = 0;
        mon->on_ground = true;
    }
}

static void monster_collide_with(Monster *mon, Entity *player) {
    // Collision (with player)
    v2 player_dir = player->pos.xz - mon->pos.xz;
    f32 player_distance = v2_length(player_dir);
    f32 penetration = mon->size.x * 0.5 + player->size.x * .5 - player_distance;

    bool collide_y = player->pos.y < mon->pos.y + mon->size.y && player->pos.y + player->size.y > mon->pos.y;
    bool collide_x = penetration > 0;

    if (collide_y && collide_x) {
        mon->pos.xz -= player_dir * penetration / player_distance;
    }
}

static void monster_wiggle(Monster *mon, Engine *eng) {
    // ==== Animation ====
    f32 speed = f_min(v3_length(mon->vel), 0.6);
    animate_exp(&mon->wiggle_amp, speed, eng->dt * 4);
    mon->wiggle_phase = f_fract(mon->wiggle_phase + mon->wiggle_amp * 0.08);
}

static void monster_die(Monster *mon, Engine *eng) {
    mon->wiggle_amp = 0;
    mon->wiggle_phase = 0;
    animate_lin(&mon->death_animation, 1, eng->dt * 4);
}

static void draw_shadow(Engine *eng, v3 shadow_pos, Image *image) {
    shadow_pos.y = SHADOW_OFFSET;

    m4 shadow_mtx = m4_id();
    m4_image_3d(&shadow_mtx, image);
    m4_rotate_x(&shadow_mtx, -R1);
    m4_translate(&shadow_mtx, shadow_pos);
    gfx_quad_3d(eng->gfx, shadow_mtx, image);
}

static Box entity_box(Entity *ent) {
    f32 h = ent->size.y;
    f32 r = ent->size.x / 2;
    Box box = {
        .min = ent->pos - (v3){r, 0, r},
        .max = ent->pos + (v3){r, h, r},
    };
    return box;
}

static void entity_collide(Engine *eng, Game *game, Entity *mon) {
    Box box = entity_box(mon);
    for (Sparse_Collision *col = sparse_set_check(game->sparse, box); col; col = col->next) {
        Entity *ent = col->node->user;
        if (ent == mon) continue;
        // gfx_debug_box(eng->gfx_dbg, col->node->box, 1);
        // gfx_debug_mtx(eng->gfx_dbg, ent->mtx);
        if (ent->is_wall) {
            m4 wall_inv = m4_invert_tr(ent->mtx);
            v3 p_local = m4_mul_pos(wall_inv, mon->pos);
            f32 rx = ent->size.x * .5;
            f32 ry = ent->size.y * .5;
            if (p_local.x < -rx) p_local.x = -rx;
            if (p_local.y < -ry) p_local.y = -ry;
            if (p_local.x > rx) p_local.x = rx;
            if (p_local.y > ry) p_local.y = ry;
            p_local.z = 0;

            v3 p_global = m4_mul_pos(ent->mtx, p_local);
            v3 dir = p_global - mon->pos;
            f32 dist = v3_length(dir);
            f32 pen = (box.max.x - box.min.x) * .5;
            // fmt_sf(OS_FMT, "D: ", dist, "\n");
            if (dist < pen) {
                mon->pos -= dir / dist * (pen - dist);
                // m4 hit = m4_id();
                // m4_translate(&hit, p_global);
                // gfx_debug_mtx(eng->gfx_dbg, hit);
            }
        }
    }
    sparse_set_add(game->sparse, box, mon);
    // gfx_debug_box(eng->gfx_dbg, box, 0);
}

static void monster_update(Monster *mon, Game *game, Engine *eng) {
    Entity *player = game->player;

    bool is_alive = mon->health > 0;

    entity_update_movement(mon, eng);

    if (is_alive) {
        monster_update_eyes(mon, eng);
        monster_update_ai(mon, game, eng);
        monster_collide_with(mon, game->player);
        entity_collide(eng, game, mon);

        monster_wiggle(mon, eng);
        v3 dir = player->pos - mon->pos;
        mon->rot.y = f_atan2(dir.x, dir.z);
        mon->rot.z = R1 * f_sin2pi(mon->wiggle_phase) * mon->wiggle_amp * 0.25;
    } else {
        monster_die(mon, eng);
        mon->rot.x = -mon->death_animation * R1;
    }

    // Update matricies
    if (mon->is_monster || mon->is_player) {
        mon->mtx = m4_id();
        m4_rotate_z(&mon->mtx, mon->rot.z);
        m4_rotate_x(&mon->mtx, mon->rot.x);
        m4_rotate_y(&mon->mtx, mon->rot.y);
        m4_translate(&mon->mtx, mon->pos);

        mon->head_mtx = m4_id();
        m4_translate_y(&mon->head_mtx, mon->size.y * .7);
        m4_apply(&mon->head_mtx, mon->mtx);
        // gfx_draw_mtx(eng, mon->head_mtx);
    }

    if (mon->image) {
        m4 mtx = m4_id();
        m4_image_3d(&mtx, mon->image);
        m4_apply(&mtx, mon->mtx);
        m4_translate_y(&mtx, MONSTER_OFFSET);
        gfx_quad_3d(eng->gfx, mtx, mon->image);
    }

    if (mon->shadow && is_alive) {
        draw_shadow(eng, mon->mtx.w, mon->shadow);
    }

    // Draw Gun
    {
        f32 aliveness = 1.0 - mon->death_animation;
        m4 mtx = m4_id();
        m4_image_3d(&mtx, game->gun);
        m4_translate_x(&mtx, -0.1 - 0.1 * mon->death_animation);
        m4_rotate_z(&mtx, -R1 * mon->death_animation * 0.2);
        m4_rotate_y(&mtx, R1 * .8 * aliveness);
        m4_translate_y(&mtx, (f32)(mon->sprite.image->size.y - mon->sprite.hand[0].y) / 32.0);
        m4_translate_x(&mtx, -(f32)mon->sprite.hand[0].x / 32.0 * 0.5f * 0.9);
        m4_apply(&mtx, mon->mtx);
        // gfx_draw_mtx(eng, mtx);
        gfx_quad_3d(eng->gfx, mtx, game->gun);
    }
}

typedef struct {
    bool hit;
    v3 pos;
    v2 uv;
    v2 pixel;
    f32 distance;
} Collide_Result;

static Collide_Result collide_quad_ray(m4 quad, Image *img, v3 ray_pos, v3 ray_dir) {
    Collide_Result result = {0};
    m4 quad_inv = m4_invert_tr(quad);
    v3 ray_pos_local = m4_mul_pos(quad_inv, ray_pos);
    v3 ray_dir_local = m4_mul_dir(quad_inv, ray_dir);
    f32 distance = ray_pos_local.z / -ray_dir_local.z;
    if (distance < 0) return result;

    v3 hit_local = ray_pos_local + ray_dir_local * distance;
    v3 hit_global = ray_pos + ray_dir * distance;

    f32 image_scale = 1.0f / 32.0f;
    v2 min = {0, 0};
    v2 max = {img->size.x * image_scale, img->size.y * image_scale};

    v2 origin = v2u_to_v2(img->origin);
    origin.y = img->size.y - origin.y - 1;

    min -= origin * image_scale;
    max -= origin * image_scale;

    if (hit_local.x > max.x) return result;
    if (hit_local.x < min.x) return result;
    if (hit_local.y > max.y) return result;
    if (hit_local.y < min.y) return result;

    v2 pixel = (hit_local.xy - min) / image_scale;
    pixel.y = img->size.y - pixel.y - 1;

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

    entity_collide(eng, game, pl);
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
        Monster *best_monster = 0;
        for (Monster *mon = game->monsters; mon; mon = mon->next) {
            if (mon == pl) continue;
            Collide_Result result = collide_quad_ray(mon->mtx, mon->image, ray_pos, ray_dir);
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

static Box m4_to_box(m4 mtx) {
    Box box = {.min = mtx.w, .max = mtx.w};
    for (u32 dz = 0; dz < 2; ++dz) {
        for (u32 dy = 0; dy < 2; ++dy) {
            for (u32 dx = 0; dx < 2; ++dx) {
                v3 p = mtx.w + dx * mtx.x + dy * mtx.y + dz * mtx.z;
                box_union_point(box, p);
            }
        }
    }
    return box;
}

static void wall_update(Game *game, Engine *eng, Entity *ent) {
    v2 size = v2u_to_v2(ent->image->size) / 32.0f;
    m4 mtx = m4_id();
    m4_image_3d(&mtx, ent->image);
    m4_apply(&mtx, ent->mtx);
    gfx_quad_3d(eng->gfx, mtx, ent->image);

    v3 p0 = m4_mul_pos(ent->mtx, (v3){-0.5 * size.x, -0.5 * size.y, 0});
    v3 p1 = m4_mul_pos(ent->mtx, (v3){0.5 * size.x, -0.5 * size.y, 0});
    v3 p2 = m4_mul_pos(ent->mtx, (v3){-0.5 * size.x, 0.5 * size.y, 0});
    v3 p3 = m4_mul_pos(ent->mtx, (v3){0.5 * size.x, 0.5 * size.y, 0});
    Box box = {p0, p0};
    box = box_union_point(box, p1);
    box = box_union_point(box, p2);
    box = box_union_point(box, p3);
    sparse_set_add(game->sparse, box, ent);
}

static void entity_update(Engine *eng, Game *game, Entity *ent) {
    if (ent->is_monster) monster_update(ent, game, eng);
    if (ent->is_player) player_update(ent, game, eng);
    if (ent->is_wall) wall_update(game, eng, ent);
    if (game->debug == DBG_Entity) gfx_debug_mtx(eng->gfx_dbg, ent->mtx);
}

static void sparse_debug_draw(Engine *eng, Sparse_Set *set, Entity *ent) {
    Sparse *sparse = set->old;
    for (u32 i = 0; i < array_count(sparse->cells); ++i) {
        for (Sparse_Cell *cell = sparse->cells[i]; cell; cell = cell->next) {
            for (Sparse_Node *node = cell->nodes; node; node = node->next) {
                if (node->user == ent) {
                    gfx_debug_box(eng->gfx_dbg, cell->box, 0);
                    for (Sparse_Node *node = cell->nodes; node; node = node->next) {
                        gfx_debug_box(eng->gfx_dbg, node->box, 1);
                        v3 p = v3i_to_v3(cell->pos) * SPARSE_BOX_SIZE;
                        gfx_debug_line(eng->gfx_dbg, p, box_center(node->box), 2);
                    }
                }
            }
        }
    }
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

    // Update bvh
    sparse_set_swap(game->sparse);
}

static void game_free(Game *game) {
    mem_free(game->mem);
}
