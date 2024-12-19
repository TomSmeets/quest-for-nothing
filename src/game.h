// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// game.h - Game data structures and implementation
#pragma once
#include "audio.h"
#include "engine.h"
#include "id.h"
#include "image.h"
#include "level.h"
#include "level_sprite.h"
#include "mem.h"
#include "monster.h"
#include "player.h"
#include "rand.h"
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

typedef struct {
    Memory *mem;
    Player *player;
    Monster *monsters;
    Level level;
    Image *gun;
} Game;

static Image *gen_gun(Memory *mem) {
    u32 size = 5;
    Image *img = image_new(mem, (v2u){size, size});
    // image_grid(img, WHITE, GRAY);
    v3 color = {0.1, 0.1, 0.1};
    image_write(img, (v2i){2, 1}, color);
    image_write(img, (v2i){3, 1}, color);
    image_write(img, (v2i){4, 1}, color);
    image_write(img, (v2i){4, 2}, color);
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
    Monster *first = 0;
    Monster *last = 0;
    for (Cell *cell = game->level.cells; cell; cell = cell->next) {
        if (!cell->y_neg) continue;
        if (v3i_eq(cell->pos, spawn)) continue;
        Monster *mon = monster_new(game->mem, rng, v3i_to_v3(cell->pos * 4));
        LIST_APPEND(first, last, mon);
    }
    game->monsters = first;
}

static Player *gen_player(Memory *mem, v3i pos) {
    Player *player = mem_struct(mem, Player);
    player->pos = v3i_to_v3(pos);
    player->old_pos = player->pos;
    player->size = (v2){0.5, 0.5};
    return player;
}

// Create a new game
static Game *game_new(Random *rng) {
    u32 level_size = 8;

    Memory *mem = mem_new();
    Game *game = mem_struct(mem, Game);
    game->mem = mem;

    // Create Level
    level_gen_outline(&game->level, rng, mem, level_size);

    // Generate player
    v3i spawn = {level_size / 2, 0, level_size / 2};
    spawn.x = 0;
    spawn.z = 0;
    game->player = gen_player(mem, spawn);
    game->gun = gen_gun(mem);

    // Generate Monsters
    game_gen_monsters(game, rng, spawn);
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

static void monster_update_physics(Monster *mon, Engine *eng) {
    // Physics
    mon->vel = (mon->pos - mon->pos_old) / eng->dt;
    mon->pos_old = mon->pos;
}

static void monster_collide_with(Monster *mon, Player *player) {
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

static void monster_draw_shadow(Monster *mon, Engine *eng) {
    v3 shadow_pos = mon->pos;
    shadow_pos.y = 0.01;

    m4 shadow_mtx = m4_id();
    m4_scale(&shadow_mtx, (v3){mon->shadow->size.x / 32.0, mon->shadow->size.x / 32.0, 1});
    m4_rotate_x(&shadow_mtx, R1);
    m4_translate(&shadow_mtx, shadow_pos);
    gfx_quad_3d(eng->gfx, shadow_mtx, mon->shadow);
}

static void monster_update(Monster *mon, Game *game, Engine *eng) {
    Player *player = game->player;

    bool is_alive = mon->health > 0;

    // Looking around
    if (mon->is_monster) {
        if (is_alive) {
            monster_update_physics(mon, eng);
            monster_update_eyes(mon, eng);
            monster_update_ai(mon, game, eng);
            monster_collide_with(mon, game->player);
            monster_wiggle(mon, eng);
            monster_draw_shadow(mon, eng);
        } else {
            monster_die(mon, eng);
        }

        mon->mtx = m4_billboard(mon->pos, mon->look_dir, mon->size, f_sin2pi(mon->wiggle_phase) * mon->wiggle_amp * 0.25 * 0.3, mon->death_animation);
    }

    gfx_quad_3d(eng->gfx, mon->mtx, mon->img);
}

typedef struct {
    bool hit;
    v3 pos;
    v2 uv;
    f32 distance;
} Collide_Result;

static Collide_Result collide_quad_ray(m4 quad, v3 ray_pos, v3 ray_dir) {
    Collide_Result result = {0};
    m4 quad_inv = m4_invert_tr(quad);
    v3 ray_pos_local = m4_mul_pos(quad_inv, ray_pos);
    v3 ray_dir_local = m4_mul_dir(quad_inv, ray_dir);
    f32 distance = ray_pos_local.z / -ray_dir_local.z;
    if (distance < 0) return result;

    v3 hit_local = ray_pos_local + ray_dir_local * distance;
    v3 hit_global = ray_pos + ray_dir * distance;
    if (hit_local.x > 0.5 || hit_local.x < -0.5) return result;
    if (hit_local.y > 0.5 || hit_local.y < -0.5) return result;
    return (Collide_Result){
        .hit = true,
        .pos = hit_global,
        .uv = hit_local.xy,
        .distance = distance,
    };
}

// Player update function
static void player_update(Player *pl, Game *game, Engine *eng) {
    f32 dt = eng->dt;
    Player_Input in = player_parse_input(eng->input);

    // ==== Input ====
    // Toggle flight
    if (in.fly) pl->flying = !pl->flying;

    // Update player head rotation
    // x -> pitch
    // y -> yaw
    // z -> roll
    pl->rot.xy += in.look.xy;

    // Limit pitch to full up and full down
    pl->rot.x = f_clamp(pl->rot.x, -0.5, 0.5);

    // wraparound yaw
    pl->rot.y = f_wrap(pl->rot.y, -1, 1);

    // Ease towards target Roll
    pl->rot.z += (in.look.z - pl->rot.z) * 10 * dt;

    // ==== Physics ====
    // Player displacement since previous frame (velocity * dt)
    v3 vel = pl->pos - pl->old_pos;
    pl->old_pos = pl->pos;
    if (pl->flying) {
        vel = v3_limit(vel, 0.01f * dt, 5.0f / 3.6f * dt);
        vel *= 1.0f - 0.2;
    } else {
        vel.xz = v2_limit(vel.xz, 0.01f * dt, 5.0f / 3.6f * dt);
        vel.xz *= 1.0f - 0.2;
    }
    pl->pos += vel;

    // Apply Gravity
    if (!pl->flying) pl->pos.y -= 9.81 * dt * dt;

    // Apply movement input
    m4 yaw_mtx = m4_id();
    m4_rotate_y(&yaw_mtx, pl->rot.y * PI); // Yaw

    v3 move = m4_mul_dir(yaw_mtx, in.move);
    move.xz = v2_limit(move.xz, 0, 1);
    move.y = in.move.y * pl->flying;
    pl->pos += move * 1.4 * dt;

    // Jumping
    if (in.jump && pl->on_ground) {
        pl->old_pos.y = pl->pos.y;
        pl->pos.y += 4 * dt;
    }

    // Ground Collision
    pl->on_ground = false;
    if (pl->pos.y <= 0 && !pl->flying) {
        pl->pos.y = 0;
        pl->on_ground = true;
    }

    // Update matricies
    pl->head_mtx = m4_id();
    m4_rotate_z(&pl->head_mtx, pl->rot.z * PI); // Roll
    m4_rotate_x(&pl->head_mtx, pl->rot.x * PI); // Pitch
    m4_rotate_y(&pl->head_mtx, pl->rot.y * PI); // Yaw
    m4_translate(&pl->head_mtx, pl->pos);
    m4_translate(&pl->head_mtx, (v3){0, .5, 0});

    pl->body_mtx = m4_id();
    m4_rotate_y(&pl->body_mtx, pl->rot.y * PI); // Yaw
    m4_translate(&pl->body_mtx, pl->pos);

    // Step sounds
    {
        f32 speed = v3_length_sq(in.move);
        if (!game->player->on_ground) speed = 0;
        pl->step_volume += (f_min(speed, 1) - pl->step_volume) * 8 * eng->dt;
    }

    // Shooting
    pl->shoot_time = animate(pl->shoot_time, -eng->dt * 4);
    if (in.shoot && pl->shoot_time == 0) {
        pl->shoot_time = 1;
        audio_play(eng->audio, 1, 0.8, rand_f32(&eng->rng) * 0.5 + 2.0);

        v3 ray_pos = pl->head_mtx.w;
        v3 ray_dir = pl->head_mtx.z;

        Collide_Result best_result = {0};
        Monster *best_monster = 0;
        for (Monster *mon = game->monsters; mon; mon = mon->next) {
            // if (mon->health == 0) continue;
            Collide_Result result = collide_quad_ray(mon->mtx, ray_pos, ray_dir);
            if (!result.hit) continue;
            if (best_result.hit && result.distance > best_result.distance) continue;
            best_monster = mon;
            best_result = result;
        }

        if (best_monster) {
            fmt_s(OS_FMT, "HIT!\n");

            Image *img = best_monster->img;

            if (best_monster->health > 0) {
                best_monster->health = 0;
                for (u32 y = 0; y < img->size.y; ++y) {
                    for (u32 x = 0; x < img->size.x; ++x) {
                        v4 *px = img->pixels + y * img->size.x + x;
                        v3 gray = {0.5f, 0.5f, 0.5f};
                        px->xyz = px->xyz * 0.4f + gray * 0.6f;
                    }
                }
            }

            for (u32 i = 0; i < 32; ++i) {
                f32 ox = rand_f32_signed(&eng->rng);
                f32 oy = rand_f32_signed(&eng->rng);

                ox = ox * ox - 0.5;
                oy = oy * oy - 0.5;

                i32 x = (best_result.uv.x + 0.5) * img->size.x + ox * best_result.distance * 4;
                i32 y = (0.5 - best_result.uv.y) * img->size.y + oy * best_result.distance * 4;
                v4 *px = image_get(img, (v2i){x, y});
                if (!px) continue;

                v3 red = best_monster->sprite.blood_color;
                px->xyz = px->xyz * 0.4f + red * 0.6f;
            }

            img->id = id_next();
        }
    }

    // Draw Gun
    {
        m4 mtx = m4_id();
        m4_translate(&mtx, (v3){-0.04, 0, 0});
        m4_scale(&mtx, 0.25f);
        m4_rotate_z(&mtx, -pl->shoot_time * R1 * 0.25);
        m4_translate(&mtx, (v3){pl->shoot_time * 0.05, 0, 0});
        m4_rotate_y(&mtx, R1);
        m4_translate(&mtx, (v3){.15, -0.12, .3});
        m4_apply(&mtx, game->player->head_mtx);
        gfx_quad_3d(eng->gfx, mtx, game->gun);
    }
}

static void cell_update(Cell *cell, Game *game, Engine *eng) {
    f32 s = 4;
    v3 x = {s, 0, 0};
    v3 y = {0, s, 0};
    v3 z = {0, 0, s};
    v3 p = v3i_to_v3(cell->pos) * s + y * .5;

    // x*=2;
    // y*=2;
    // z*=2;
    // p*=2;

    if (cell->x_neg) gfx_quad_3d(eng->gfx, (m4){z, y, -x, p - x * .5}, cell->x_neg);
    if (cell->x_pos) gfx_quad_3d(eng->gfx, (m4){-z, y, x, p + x * .5}, cell->x_pos);

    if (cell->z_pos) gfx_quad_3d(eng->gfx, (m4){x, y, z, p + z * .5}, cell->z_pos);
    if (cell->z_neg) gfx_quad_3d(eng->gfx, (m4){-x, y, -z, p - z * .5}, cell->z_neg);

    // OK
    if (cell->y_neg) gfx_quad_3d(eng->gfx, (m4){x, z, -y, p - y * .5}, cell->y_neg);
    if (cell->y_pos) gfx_quad_3d(eng->gfx, (m4){x, -z, y, p + y * .5}, cell->y_pos);

    // v3i dx = cell->direction->pos - cell->pos;
    // if(cell->direction) {
    //     m4 mtx = m4_id();
    //     m4_rotate_x(&mtx, R1);
    //     m4_rotate_y(&mtx, -R1*(cell->direction-1));
    //     m4_translate(&mtx, v3i_to_v3(cell->pos) + y *.1);
    //     gfx_quad_3d(eng->gfx, mtx, eng->image_arrow);
    // }
}

static void game_update(Game *game, Engine *eng) {
    player_update(game->player, game, eng);

    for (Monster *mon = game->monsters; mon; mon = mon->next) {
        monster_update(mon, game, eng);
    }

    for (Cell *cell = game->level.cells; cell; cell = cell->next) {
        cell_update(cell, game, eng);
    }

    // fmt_sfff(OS_FMT, "Player: ", game->player->pos.x, ", ", game->player->pos.y, ", ", game->player->pos.z, "\n");
}

static void game_free(Game *game) {
    mem_free(game->mem);
}
