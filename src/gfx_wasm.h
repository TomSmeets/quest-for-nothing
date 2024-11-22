// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gfx.h - Gfx implementation for the web
#pragma once
#include "gfx_api.h"
#include "os.h"

WASM_IMPORT(js_gfx_grab) void js_gfx_grab(bool grab);

struct Gfx {
    Input input;
    Input input_copy;
};

static Gfx GFX_GLOBAL;

static Gfx *os_gfx_init(Memory *mem, char *title) {
    return &GFX_GLOBAL;
}

static Input *os_gfx_poll(Gfx *gfx) {
    Input *input = &gfx->input;

    // Atomic copy of input, js will get events while we are updating
    gfx->input_copy = *input;

    // Reset input
    input_reset(input);

    return &gfx->input_copy;
}

static void os_gfx_set_mouse_grab(Gfx *gfx, bool grab) {
    js_gfx_grab(grab);
    gfx->input.mouse_is_grabbed = grab;
}

static void os_gfx_begin(Gfx *gfx) {
}
static void os_gfx_quad(Gfx *gfx, m4 mtx, Image *img, bool ui) {
}
static void os_gfx_end(Gfx *gfx, m4 camera) {
}

void js_gfx_key_down(u32 key, bool down) {
    fmt_suu(OS_FMT, "KEY: key=", key, " down=", down, "\n");
    input_emit(&GFX_GLOBAL.input, key_from_char(key), down);
}

void js_gfx_mouse_move(f32 x, f32 y, f32 dx, f32 dy) {
    fmt_sffff(OS_FMT, "Mouse: x=", x, " y=", y, " dx=", dx, " dy=", dy, "\n");
    GFX_GLOBAL.input.mouse_moved = true;
    GFX_GLOBAL.input.mouse_pos = (v2i){x, y};
    GFX_GLOBAL.input.mouse_rel = (v2i){dx, dy};
}

void js_gfx_mouse_down(u32 button, bool down) {
    // Yeah, i don't like the printf at all, lets get rid of it
    // I like the fmt idea with optional output buffer
    fmt_suu(OS_FMT, "Mouse: button=", button, " down=", down, "\n");
    if (button == 0) input_emit(&GFX_GLOBAL.input, KEY_MOUSE_LEFT, down);
    if (button == 2) input_emit(&GFX_GLOBAL.input, KEY_MOUSE_RIGHT, down);
}
