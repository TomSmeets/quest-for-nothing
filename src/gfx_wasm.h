// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gfx.h - Gfx implementation for the web
#pragma once
#include "gfx_api.h"
#include "os.h"

WASM_IMPORT(js_gfx_grab) void js_gfx_grab(bool grab);
WASM_IMPORT(js_gfx_begin) void js_gfx_begin(void);
WASM_IMPORT(js_gfx_end)   void js_gfx_end(void);

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

void js_gfx_resize(i32 width, i32 height) {
    fmt_suu(OS_FMT, "Resize: width=", width, " height=", height, "\n");
}
