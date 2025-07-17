// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// game_debug.h - Debug drawing modes for QFN
#pragma once
#include "qfn/engine.h"

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

// DBG_Texture
static void debug_draw_texture(Engine *eng) {
    // *gfx_pass_quad(eng->gfx, &eng->gfx->pass_ui) = (Gfx_Quad){
    //     .x = {400, 0, 0},
    //     .y = {0, 400, 0},
    //     .z = {0, 0, 1},
    //     .w = {0, 0, 0},
    //     .uv_pos = {0, 0},
    //     .uv_size = {1, 1},
    // };

    // Text
    m4 mtx = m4_id();
    m4_translate_x(&mtx, -eng->input->window_size.x * .5 + 3 * 4);
    m4_translate_y(&mtx, eng->input->window_size.y * .5 - 3 * 4);
    gfx_draw_ui(eng->gfx, mtx, eng->ui->image);
    ui_text(eng->ui, mtx, "Hello World!\n0123456789\n3.141592");
}
