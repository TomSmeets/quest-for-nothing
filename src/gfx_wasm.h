// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// gfx_wasm.h - GFX Platform implementation using WebGL2
#pragma once
#include "os_wasm.h"
WASM_IMPORT(js_gfx_grab) void js_gfx_grab(bool grab);
WASM_IMPORT(js_gfx_fullscreen) void js_gfx_fullscreen(bool fullscreen);
WASM_IMPORT(js_gfx_begin) void js_gfx_begin(void);
WASM_IMPORT(js_gfx_init) void js_gfx_init(void);
WASM_IMPORT(js_gfx_texture) void js_gfx_texture(u32 x, u32 y, u32 sx, u32 sy, void *pixels);
WASM_IMPORT(js_gfx_draw) void js_gfx_draw(float *projection, bool depth, u32 quad_count, OS_Gfx_Quad *quad_list);
WASM_IMPORT(js_gfx_end) void js_gfx_end(void);

struct OS_Gfx {
    Input input;
    Input input_copy;
    v2 audio_buffer[1024];
};

// WASM allows for globals, there is no reload anyway
static OS_Gfx GFX_GLOBAL;

// Initialize Graphics stack
static OS_Gfx *os_gfx_init(Memory *mem, char *title) {
    js_gfx_init();
    return &GFX_GLOBAL;
}

// Start frame
static Input *os_gfx_begin(OS_Gfx *gfx) {
    // Atomic copy of input, js will get events while we are updating
    gfx->input_copy = gfx->input;

    // Reset input for next frame
    input_reset(&gfx->input);

    // Start frame
    js_gfx_begin();
    return &gfx->input_copy;
}

// Grab mouse
static void os_gfx_set_grab(OS_Gfx *gfx, bool grab) {
    js_gfx_grab(grab);
    gfx->input.mouse_is_grabbed = grab;
}

static void os_gfx_set_fullscreen(OS_Gfx *gfx, bool full) {
    js_gfx_fullscreen(full);
    gfx->input.is_fullscreen = full;
}

// Write to texture atlas
static void os_gfx_texture(OS_Gfx *gfx, v2u pos, Image *img) {
    js_gfx_texture(pos.x, pos.y, img->size.x, img->size.y, img->pixels);
}

// Perform a draw call
static void os_gfx_draw(OS_Gfx *gfx, m44 projection, bool depth, u32 quad_count, OS_Gfx_Quad *quad_list) {
    js_gfx_draw((float *)&projection, depth, quad_count, quad_list);
}

// Finish frame
static void os_gfx_end(OS_Gfx *gfx) {
    js_gfx_end();
}

// Callbacks from js
void js_gfx_key_down(u32 key, bool down) {
    // fmt_suu(OS_FMT, "KEY: key=", key, " down=", down, "\n");
    input_emit(&GFX_GLOBAL.input, key_from_char(key), down);
}

void js_gfx_mouse_move(f32 x, f32 y, f32 dx, f32 dy) {
    Input *input = &GFX_GLOBAL.input;
    // fmt_sffff(OS_FMT, "Mouse: x=", x, " y=", y, " dx=", dx, " dy=", dy, "\n");
    input->mouse_moved = true;
    input->mouse_pos.x = x - (f32)input->window_size.x / 2.0;
    input->mouse_pos.y = input->window_size.y / 2.0f - y;
    input->mouse_rel.x += dx;
    input->mouse_rel.y -= dy;
}

void js_gfx_mouse_down(u32 button, bool down) {
    // fmt_suu(OS_FMT, "Mouse: button=", button, " down=", down, "\n");
    if (button == 0) input_emit(&GFX_GLOBAL.input, KEY_MOUSE_LEFT, down);
    if (button == 2) input_emit(&GFX_GLOBAL.input, KEY_MOUSE_RIGHT, down);
}

void js_gfx_resize(i32 width, i32 height) {
    // fmt_suu(OS_FMT, "Resize: width=", width, " height=", height, "\n");
    GFX_GLOBAL.input.window_size.x = width;
    GFX_GLOBAL.input.window_size.y = height;
}

v2 *js_audio_callback(u32 sample_count) {
    assert(sample_count == array_count(GFX_GLOBAL.audio_buffer), "Invalid sample count");
    os_gfx_audio_callback(sample_count, GFX_GLOBAL.audio_buffer);
    return GFX_GLOBAL.audio_buffer;
}
