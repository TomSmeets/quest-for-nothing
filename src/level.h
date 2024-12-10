#pragma once
#include "image.h"
#include "level_sprite.h"
#include "mem.h"
#include "monster.h"
#include "player.h"
#include "rand.h"
#include "types.h"
#include "vec.h"

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

static Cell *cell_new(Memory *mem, v3i pos) {
    Cell *cell = mem_struct(mem, Cell);
    cell->pos = pos;
    return cell;
}

typedef struct {
    Cell *cells;
} Level;

static Cell *level_get(Level *level, v3i pos) {
    for (Cell *cell = level->cells; cell; cell = cell->next) {
        if (v3i_eq(cell->pos, pos)) {
            return cell;
        }
    }
    return 0;
}

// Generate an empty level
static void level_gen_outline(Level *level, Random *rng, Memory *mem, u32 size) {
    assert(level->cells == 0, "Level was already generated");
    Cell *first = 0;
    Cell *last = 0;

    u32 sx = size;
    u32 sz = size;
    u32 sy = 2;

    // Generate Empty Cells
    for (u32 y = 0; y < sy; ++y) {
        for (u32 z = 0; z < sz; ++z) {
            for (u32 x = 0; x < sx; ++x) {
                Cell *cell = cell_new(mem, (v3i){x, y, z});
                LIST_APPEND(first, last, cell);

                if (x == 0) cell->x_neg = level_sprite_generate(mem, rng);
                if (x == sx - 1) cell->x_pos = level_sprite_generate(mem, rng);
                if (z == 0) cell->z_neg = level_sprite_generate(mem, rng);
                if (z == sz - 1) cell->z_pos = level_sprite_generate(mem, rng);

                if (y == 0) cell->y_neg = level_sprite_generate(mem, rng);
                if (y == sy - 1) cell->y_pos = level_sprite_generate(mem, rng);
            }
        }
    }

    level->cells = first;
}
