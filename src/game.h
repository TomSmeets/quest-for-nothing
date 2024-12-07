// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// game.h - Game data structures and implementation
#pragma once
#include "audio.h"
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

    f32 shoot_time;
    f32 step_volume;
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

static void game_update(Game *game, Audio *audio, Gfx *gfx, Input *input, f32 dt) {
    Player_Input in = player_parse_input(input);
    player_update(game->player, dt, &in);

    // Step sounds
    {
        f32 speed = v3_length_sq(in.move);
        if (!game->player->on_ground) speed = 0;
        game->step_volume += (f_min(speed, 1) - game->step_volume) * 8 * dt;
    }

    // Shooting
    if (key_click(input, KEY_MOUSE_LEFT)) {
        game->shoot_time = 1;
        audio_play(audio, 1, 0.8, rand_f32(&game->rng) * 0.5 + 2.0);
    }

    if (game->shoot_time > 0)
        game->shoot_time -= dt * 4;
    else
        game->shoot_time = 0;

    for (Monster *mon = game->monsters; mon; mon = mon->next) {
        monster_update(mon, dt, game->player, &game->rng, gfx);
    }

    // Draw level
    for (Cell *cell = game->level.cells; cell; cell = cell->next) {
        v3 x = {1, 0, 0};
        v3 y = {0, 1, 0};
        v3 z = {0, 0, 1};
        v3 p = v3i_to_v3(cell->pos);
        if (cell->x_neg) gfx_quad_3d(gfx, (m4){-z, y, x, p - x * .5}, cell->x_neg);
        if (cell->z_neg) gfx_quad_3d(gfx, (m4){x, y, z, p - z * .5}, cell->z_neg);

        if (cell->x_pos) gfx_quad_3d(gfx, (m4){z, y, -x, p + x * .5}, cell->x_pos);
        if (cell->z_pos) gfx_quad_3d(gfx, (m4){x, y, z, p + z * .5}, cell->z_pos);

        // OK
        if (cell->y_neg) gfx_quad_3d(gfx, (m4){x, z, -y, p}, cell->y_neg);
        if (cell->y_pos) gfx_quad_3d(gfx, (m4){x, -z, y, p + y}, cell->y_pos);
    }

    // Draw Gun
    {
        m4 mtx = m4_id();
        m4_translate(&mtx, (v3){-2.0f / 5.0f, 0, 0});
        m4_scale(&mtx, 0.2f);
        m4_rotate_z(&mtx, -game->shoot_time * R1 * 0.25);
        m4_translate(&mtx, (v3){game->shoot_time * 0.05, 0, 0});
        m4_rotate_y(&mtx, R1);
        m4_translate(&mtx, (v3){.17, -0.12, .2});
        m4_apply(&mtx, game->player->head_mtx);
        gfx_quad_3d(gfx, mtx, game->gun);
    }
}

static void game_free(Game *game) {
    mem_free(game->mem);
}
