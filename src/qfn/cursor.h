// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// cursor.h - Mouse cursor handling
#pragma once
#include "gfx.h"
#include "image.h"
#include "math.h"
#include "monster.h"
#include "ui.h"

typedef struct {
    // Mouse cursor
    Image *image;
} Cursor;

static void cursor_load(Cursor *cursor, Memory *mem) {
    u32 size = 5;
    Image *img = image_new(mem, (v2u){size, size});
    for (u32 i = 0; i < size; ++i) {
        v4 col = color_alpha(WHITE, 1);
        image_write4(img, (v2i){i, i}, col);
        image_write4(img, (v2i){i, size - 1 - i}, col);
    }
    cursor->image = img;
}

static void cursor_draw(Cursor *cursor, Engine *eng) {
    Input *input = eng->input;
    Gfx *gfx = eng->gfx;

    // Capture Mouse
    if (key_click(input, KEY_MOUSE_LEFT)) {
        gfx_set_grab(eng->gfx, true);
    }

    // Release Grab on focus lost or Esc
    if ((input->focus_lost || key_click(input, KEY_ESCAPE)) && input->mouse_is_grabbed) {
        gfx_set_grab(eng->gfx, false);
    }

    // Grab with G
    if (key_click(input, KEY_G)) {
        gfx_set_grab(eng->gfx, !input->mouse_is_grabbed);
    }

    m4 mtx = m4_id();
    m4_image_ui(&mtx, cursor->image);
    if (!input->mouse_is_grabbed) {
        m4_translate(&mtx, (v3){input->mouse_pos.x, input->mouse_pos.y, 0});
    }
    gfx_quad_ui(gfx, mtx, cursor->image);
}
