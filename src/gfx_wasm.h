// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gfx.h - Gfx implementation for the web
#pragma once
#include "gfx_api.h"
#include "os.h"

WASM_IMPORT(js_gfx_grab) void js_gfx_grab(bool grab);

struct Gfx {
    Input input;
};

static Gfx GFX_GLOBAL;

static Gfx *os_gfx_init(Memory *mem, char *title) {
    return &GFX_GLOBAL;
}

static Input *os_gfx_poll(Gfx *gfx) {
    return &gfx->input;
}

static void os_gfx_set_mouse_grab(Gfx *gfx, bool grab) {
    js_gfx_grab(grab);
}

static void os_gfx_begin(Gfx *gfx, m4s *proj) {
}
static void os_gfx_quad(Gfx *gfx, m4s *mtx, Image *img) {
}
static void os_gfx_end(Gfx *gfx) {
}

void js_gfx_keydown(u32 key) {
    os_printf("KEY: %u Down\n",key);
    input_emit(&GFX_GLOBAL.input, key_from_char(key), true);
}

void js_gfx_keyup(u32 key) {
    os_printf("KEY: %u Up\n",key);
    input_emit(&GFX_GLOBAL.input, key_from_char(key), false);
}

void js_gfx_mouse_move(f32 x, f32 y, f32 dx, f32 dy) {
    os_printf("Mouse: x=%f y=%f dx=%f dy=%f\n",x, y, dx, dy);
    GFX_GLOBAL.input.mouse_moved = true;
    GFX_GLOBAL.input.mouse_pos = (v2i){x, y};
    GFX_GLOBAL.input.mouse_rel = (v2i){dx, dy};
}
