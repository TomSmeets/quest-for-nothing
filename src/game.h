// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// game.h - Game data structures and implementation
#pragma once
#include "audio.h"
#include "engine.h"
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
    Random rng;
    Level level;
    Monster *monsters;
    Player *player;
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

static void game_gen_monsters(Game *game, v3i spawn) {
    Monster *first = 0;
    Monster *last = 0;
    for (Cell *cell = game->level.cells; cell; cell = cell->next) {
        if (v3i_eq(cell->pos, spawn)) continue;
        Monster *mon = monster_new(game->mem, &game->rng, v3i_to_v3(cell->pos));
        LIST_APPEND(first, last, mon);
    }
    game->monsters = first;
}

static Player *gen_player(Memory *mem, v3i pos) {
    Player *player = mem_struct(mem, Player);
    player->pos = v3i_to_v3(pos);
    player->old_pos = player->pos;
    return player;
}

// Create a new game
static Game *game_new(void) {
    u32 level_size = 32;

    Memory *mem = mem_new();
    Game *game = mem_struct(mem, Game);
    game->mem = mem;

    // Create Level
    level_gen_outline(&game->level, mem, level_size);
    level_gen_indoor(&game->level, mem, &game->rng);

    // Generate player
    v3i spawn = {level_size / 2, 0, level_size / 2};
    spawn.x = 0;
    spawn.z = 0;
    game->player = gen_player(mem, spawn);
    game->gun = gen_gun(mem);

    // Generate Monsters
    game_gen_monsters(game, spawn);
    return game;
}

static void monster_update(Monster *mon, Game *game, Engine *eng) {
    // ==== Physics ====
    v3 vel = mon->pos - mon->old_pos;
    mon->old_pos = mon->pos;

    // Apply movement
    mon->pos.xz += mon->move_dir * eng->dt;

    mon->move_time -= eng->dt;
    if (mon->move_time <= 0) {
        mon->move_time = rand_f32_range(&eng->rng, 2, 10);

        u32 mode = rand_u32_range(&eng->rng, 0, 2);
        // fmt_su(OS_FMT, "mode=", mode, "\n");
        if (mode == 0) mon->move_dir = 0;
        if (mode == 1) mon->move_dir = v2_normalize(game->player->pos.xz - mon->pos.xz) * 0.1;
        if (mode == 2) mon->move_dir = v2_from_rot(rand_f32_signed(&eng->rng) * PI) * 0.25;
        monster_sprite_update_eyes(&mon->sprite, &eng->rng);
    }

    // Player Collision
    v2 player_dir = game->player->pos.xz - mon->pos.xz;
    f32 player_distance = v2_length(player_dir);

    float radius = 0.5;
    if (player_distance < radius) {
        mon->pos.xz -= player_dir * (radius - player_distance) / player_distance;
    }

    // ==== Animation ====
    f32 speed = v3_length(vel);
    mon->speed += (speed / eng->dt - mon->speed) * eng->dt;
    mon->wiggle += speed * 2;
    mon->wiggle = f_fract(mon->wiggle);
    mon->body_mtx = m4_billboard(mon->pos, game->player->pos, f_sin2pi(mon->wiggle) * f_min(mon->speed * 0.5, 0.2) * 0.3);

    mon->sprite_mtx = m4_id();
    m4_translate(&mon->sprite_mtx, (v3){0, 0.5, 0});
    m4_scale(&mon->sprite_mtx, (v3){(f32)mon->sprite.image->size.x / 32.0f, (f32)mon->sprite.image->size.y / 32.0f, 1});
    m4_apply(&mon->sprite_mtx, mon->body_mtx);
    gfx_quad_3d(eng->gfx, mon->sprite_mtx, mon->sprite.image);
}

static f32 animate(f32 x, f32 dt) {
    x -= dt;
    if (x <= 0) return 0;
    return x;
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
    if (pl->pos.y <= 0) {
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
    pl->shoot_time = animate(pl->shoot_time, eng->dt * 4);
    if (in.shoot_single && pl->shoot_time == 0) {
        pl->shoot_time = 1;
        audio_play(eng->audio, 1, 0.8, rand_f32(&game->rng) * 0.5 + 2.0);
    }

    // Draw Gun
    {
        m4 mtx = m4_id();
        m4_translate(&mtx, (v3){-2.0f / 5.0f, 0, 0});
        m4_scale(&mtx, 0.2f);
        m4_rotate_z(&mtx, -pl->shoot_time * R1 * 0.25);
        m4_translate(&mtx, (v3){pl->shoot_time * 0.05, 0, 0});
        m4_rotate_y(&mtx, R1);
        m4_translate(&mtx, (v3){.17, -0.12, .2});
        m4_apply(&mtx, game->player->head_mtx);
        gfx_quad_3d(eng->gfx, mtx, game->gun);
    }
}

static void cell_update(Cell *cell, Game *game, Engine *eng) {
    v3 x = {1, 0, 0};
    v3 y = {0, 1, 0};
    v3 z = {0, 0, 1};
    v3 p = v3i_to_v3(cell->pos);
    if (cell->x_neg) gfx_quad_3d(eng->gfx, (m4){-z, y, x, p - x * .5}, cell->x_neg);
    if (cell->z_neg) gfx_quad_3d(eng->gfx, (m4){x, y, z, p - z * .5}, cell->z_neg);

    if (cell->x_pos) gfx_quad_3d(eng->gfx, (m4){z, y, -x, p + x * .5}, cell->x_pos);
    if (cell->z_pos) gfx_quad_3d(eng->gfx, (m4){x, y, z, p + z * .5}, cell->z_pos);

    // OK
    if (cell->y_neg) gfx_quad_3d(eng->gfx, (m4){x, z, -y, p}, cell->y_neg);
    if (cell->y_pos) gfx_quad_3d(eng->gfx, (m4){x, -z, y, p + y}, cell->y_pos);
}

static void game_update(Game *game, Engine *eng) {
    player_update(game->player, game, eng);

    for (Monster *mon = game->monsters; mon; mon = mon->next) {
        monster_update(mon, game, eng);
    }

    for (Cell *cell = game->level.cells; cell; cell = cell->next) {
        cell_update(cell, game, eng);
    }
}

static void game_free(Game *game) {
    mem_free(game->mem);
}
