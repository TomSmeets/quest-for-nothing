// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// game.h - Store game data structures
#pragma once
#include "image.h"
#include "mem.h"
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

typedef struct Cell {
    v3i pos;
    bool generated;
    Image *x_pos;
    Image *x_neg;
    Image *y_pos;
    Image *y_neg;
    Image *z_pos;
    Image *z_neg;
    struct Cell *next;
} Cell;

typedef struct Monster {
    v3 pos;
    u32 health;
    Image *image;
    struct Monster *next;
} Monster;

typedef struct {
    Memory *mem;
    Random rng;
    Cell *level;
    Monster *monsters;
    Player *player;
} Game;

static Cell *game_cell_get(Game *game, v3i pos) {
    for (Cell *cell = game->level; cell; cell = cell->next) {
        if (v3i_eq(cell->pos, pos)) {
            return cell;
        }
    }
    return 0;
}

static v4 rand_color(Random *rng) {
    v4 ret;
    ret.x = rand_f32(rng);
    ret.y = rand_f32(rng);
    ret.z = rand_f32(rng);
    ret.w = 1;
    return ret;
}

static Image *gen_wall_image(Memory *mem, Random *rng) {
    Image *img = image_new(mem, (v2u){32, 32});
    image_grid(img, rand_color(rng), rand_color(rng));
    return img;
}

static Cell *cell_new(Memory *mem, v3i pos) {
    Cell *cell = mem_struct(mem, Cell);
    cell->pos = pos;
    return cell;
}

static Monster *monster_new(Memory *mem, Random *rng, v3 pos) {
    Monster *monster = mem_struct(mem, Monster);
    monster->pos = pos;
    monster->health = 10;
    monster->image = image_new(mem, (v2u){32, 32});
    image_grid(monster->image, rand_color(rng), rand_color(rng));
    return monster;
}

// Generate an empty level
static Cell *gen_level_outline(Memory *mem, u32 size) {
    Cell *first = 0;
    Cell *last = 0;

    // Generate Empty Cells
    for (u32 z = 0; z < size; ++z) {
        for (u32 x = 0; x < size; ++x) {
            Cell *cell = cell_new(mem, (v3i){x, 0, z});
            LIST_APPEND(first, last, cell);
        }
    }
    return first;
}

static void gen_indoor(Cell *level, Memory *mem, Random *rng) {
    for (Cell *cell = level; cell; cell = cell->next) {
        if (1) cell->x_neg = gen_wall_image(mem, rng);
        if (1) cell->x_pos = gen_wall_image(mem, rng);
        if (1) cell->z_pos = gen_wall_image(mem, rng);
        if (1) cell->z_neg = gen_wall_image(mem, rng);
        if (1) cell->y_pos = gen_wall_image(mem, rng);
        if (1) cell->y_neg = gen_wall_image(mem, rng);
    }
}

static Monster *gen_monsters(Cell *level, Random *rand, v3i player_pos, Memory *mem) {
    Monster *first = 0;
    Monster *last = 0;

    for (Cell *cell = level; cell; cell = cell->next) {
        if (v3i_eq(cell->pos, player_pos)) continue;

        Monster *mon = monster_new(mem, rand, v3i_to_v3(cell->pos));
        LIST_APPEND(first, last, mon);
    }

    return first;
}

static Player *gen_player(Memory *mem, v3i pos) {
    Player *player = mem_struct(mem, Player);
    player->pos = v3i_to_v3(pos);
    player->old_pos = player->pos;
    return player;
}

// Create a new game
static Game *game_new(void) {
    u32 level_size = 16;

    Memory *mem = mem_new();
    Game *game = mem_struct(mem, Game);
    game->mem = mem;

    // Create Level
    game->level = gen_level_outline(mem, level_size);
    gen_indoor(game->level, mem, &game->rng);

    // Generate player
    v3i spawn = {level_size / 2, 0, level_size / 2};
    game->player = gen_player(mem, spawn);

    // Generate Monsters
    game->monsters = gen_monsters(game->level, &game->rng, spawn, mem);
    return game;
}

static void game_update(Game *game, f32 dt) {
}

static void game_free(Game *game) {
    mem_free(game->mem);
}
