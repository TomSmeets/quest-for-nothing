// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// level.h - Game level
#pragma once
#include "image.h"
#include "level_sprite.h"
#include "mem.h"
#include "monster.h"
#include "player.h"
#include "rand.h"
#include "types.h"
#include "vec.h"

// Ideas
//   Generate in layers, layers give depth
//   Pure white noise is boring, but by layering at different scales sometihng interesting is created.
// Referneces

typedef struct Cell Cell;
struct Cell {
    v3i pos;
    Cell *direction;
    Image *x_pos;
    Image *x_neg;
    Image *y_pos;
    Image *y_neg;
    Image *z_pos;
    Image *z_neg;
    Cell *next;
};

static Cell *cell_new(Memory *mem, v3i pos) {
    Cell *cell = mem_struct(mem, Cell);
    cell->pos = pos;
    return cell;
}

typedef struct {
    Memory *mem;
    Cell *cells;
    Cell *cells_end;
} Level;

static Cell *level_get(Level *level, v3i pos) {
    for (Cell *cell = level->cells; cell; cell = cell->next) {
        if (v3i_eq(cell->pos, pos)) {
            return cell;
        }
    }
    return 0;
}

static Cell *level_new(Level *level, v3i pos) {
    Cell *cell = level_get(level, pos);
    if (cell) return cell;

    cell = cell_new(level->mem, pos);
    LIST_APPEND(level->cells, level->cells_end, cell);
    return cell;
}

static void level_gen_maze(Level *level, Random *rng, Cell *cell) {
    v3i dirs[4] = {
        {+1, 0, 0},
        {0, 0, +1},
        {-1, 0, 0},
        {0, 0, -1},
    };
    u32 dir_count = array_count(dirs);

    for (;;) {
        // failed, find previous
        if (dir_count == 0) break;

        u32 i = rand_u32_range(rng, 0, dir_count - 1);
        v3i dir = dirs[i];
        dirs[i] = dirs[--dir_count];

        Cell *next = level_get(level, cell->pos + dir);
        if (!next) continue;
        if (next->direction) continue;
        next->direction = cell;
        level_gen_maze(level, rng, next);
    }
}

// Generate an empty level
static void level_gen_outline(Level *level, Random *rng, Memory *mem, u32 size) {
    assert(level->cells == 0, "Level was already generated");
    u32 sx = size;
    u32 sz = size;
    u32 sy = 1;

    // Generate Empty Cells
    for (u32 y = 0; y < sy; ++y) {
        for (u32 z = 0; z < sz; ++z) {
            for (u32 x = 0; x < sx; ++x) {
                Cell *cell = cell_new(mem, (v3i){x, y, z});
                LIST_APPEND(level->cells, level->cells_end, cell);
            }
        }
    }

    Cell *start = level->cells;
    start->direction = start;
    level_gen_maze(level, rng, start);

    // Place walls
    Image *wall = level_sprite_generate(mem, rng);
    Image *floor = level_sprite_generate(mem, rng);
    for (Cell *cell = level->cells; cell; cell = cell->next) {
        v3i dx = {1, 0, 0};
        v3i dz = {0, 0, 1};
        if (cell->pos.y == 0) {
            Cell *cell_xp = level_get(level, cell->pos + dx);
            Cell *cell_zp = level_get(level, cell->pos + dz);
            Cell *cell_xn = level_get(level, cell->pos - dx);
            Cell *cell_zn = level_get(level, cell->pos - dz);

            bool door_xp = cell->direction == cell_xp || (cell_xp && cell_xp->direction == cell);
            bool door_zp = cell->direction == cell_zp || (cell_zp && cell_zp->direction == cell);
            bool door_xn = cell->direction == cell_xn || (cell_xn && cell_xn->direction == cell);
            bool door_zn = cell->direction == cell_zn || (cell_zn && cell_zn->direction == cell);

            if (!door_xp) cell->x_pos = wall;
            if (!door_zp) cell->z_pos = wall;
            if (!door_xn) cell->x_neg = wall;
            if (!door_zn) cell->z_neg = wall;
        }

        if (cell->pos.y == 0) cell->y_neg = floor;
        if (cell->pos.y == (i32)sy - 1) cell->y_pos = wall;
    }
}
