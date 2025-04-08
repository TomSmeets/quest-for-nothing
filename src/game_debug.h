#pragma once
#include "engine.h"
#include "entity.h"
#include "sparse_set.h"

static void sparse_debug_draw(Engine *eng, Sparse_Set *set, Entity *ent) {
    Sparse *sparse = set->old;
    for (u32 i = 0; i < array_count(sparse->cells); ++i) {
        for (Sparse_Cell *cell = sparse->cells[i]; cell; cell = cell->next) {
            for (Sparse_Node *node = cell->nodes; node; node = node->next) {
                if (node->user == ent) {
                    gfx_debug_box(eng->gfx_dbg, cell->box, 0);
                    for (Sparse_Node *node = cell->nodes; node; node = node->next) {
                        gfx_debug_box(eng->gfx_dbg, node->box, 1);
                        v3 p = v3i_to_v3(cell->pos) * SPARSE_CELL_SIZE;
                        gfx_debug_line(eng->gfx_dbg, p, box_center(node->box), 2);
                    }
                }
            }
        }
    }
}
