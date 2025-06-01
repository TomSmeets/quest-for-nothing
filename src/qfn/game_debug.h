#pragma once
#include "qfn/engine.h"
#include "qfn/entity.h"
#include "qfn/sparse_set.h"

// List of debug visulisation modes
typedef enum {
    // Normal gameplay
    DBG_None,

    // Draw entity transformation matricies
    DBG_Entity,

    // Draw texture atlas
    DBG_Texture,

    // Show Collision bounding boxes
    DBG_Collision,
    DBG_COUNT,
} Game_Debug;

// Cycle debug mode
static void debug_next(Game_Debug *debug) {
    *debug = (*debug + 1) % DBG_COUNT;
}

// DBG_Entity,
static void debug_draw_entity(Engine *eng, Entity *ent) {
    gfx_debug_mtx(eng->gfx_dbg, ent->image_mtx);
}

// DBG_Collision
static void debug_draw_collisions(Engine *eng, Sparse_Set *set, Entity *ent) {
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

// DBG_Texture
static void debug_draw_texture(Engine *eng) {
    *gfx_pass_quad(eng->gfx, &eng->gfx->pass_ui) = (Gfx_Quad){
        .x = {400, 0, 0},
        .y = {0, 400, 0},
        .z = {0, 0, 1},
        .w = {0, 0, 0},
        .uv_pos = {0, 0},
        .uv_size = {1, 1},
    };

    // Text
    m4 mtx = m4_id();
    m4_translate_x(&mtx, -eng->input->window_size.x * .5 + 3 * 4);
    m4_translate_y(&mtx, eng->input->window_size.y * .5 - 3 * 4);
    gfx_quad_ui(eng->gfx, mtx, eng->ui->image);
    ui_text(eng->ui, mtx, "Hello World!\n0123456789\n3.141592");
}
