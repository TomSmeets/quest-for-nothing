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
static void level_gen_outline(Level *level, Memory *mem, u32 size) {
    assert(level->cells == 0, "Level was already generated");
    Cell *first = 0;
    Cell *last = 0;

    // Generate Empty Cells
    for (u32 z = 0; z < size; ++z) {
        for (u32 x = 0; x < size; ++x) {
            Cell *cell = cell_new(mem, (v3i){x, 0, z});
            LIST_APPEND(first, last, cell);
        }
    }

    level->cells = first;
}

static void level_gen_indoor(Level *level, Memory *mem, Random *rng) {
    for (Cell *cell = level->cells; cell; cell = cell->next) {
        if (0) cell->x_neg = level_sprite_generate(mem, rng);
        if (0) cell->x_pos = level_sprite_generate(mem, rng);
        if (0) cell->z_pos = level_sprite_generate(mem, rng);
        if (0) cell->z_neg = level_sprite_generate(mem, rng);
        if (1) cell->y_pos = level_sprite_generate(mem, rng);
        if (1) cell->y_neg = level_sprite_generate(mem, rng);
    }
}

