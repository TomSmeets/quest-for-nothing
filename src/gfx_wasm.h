// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// gfx_wasm.h - GFX Platform implementation using WebGL2
#pragma once
#include "gfx.h"
#include "os_wasm.h"
WASM_IMPORT(gfx_js_grab) void gfx_js_grab(bool grab);
WASM_IMPORT(gfx_js_fullscreen) void gfx_js_fullscreen(bool fullscreen);
WASM_IMPORT(gfx_js_begin) void gfx_js_begin(void);
WASM_IMPORT(gfx_js_init) void gfx_js_init(void);
WASM_IMPORT(gfx_js_texture) void gfx_js_texture(u32 x, u32 y, u32 sx, u32 sy, void *pixels);
WASM_IMPORT(gfx_js_draw) void gfx_js_draw(float *projection, bool depth, u32 quad_count, Gfx_Quad *quad_list);
WASM_IMPORT(gfx_js_end) void gfx_js_end(void);

struct Gfx_Imp {
    Input input;
    Input input_copy;
    v2 audio_buffer[1024];
};

// WASM allows for globals, there is no reload anyway
static Gfx_Imp GFX_GLOBAL;

// Initialize Graphics stack
static Gfx_Imp *gfx_imp_init(Memory *mem, char *title) {
    gfx_js_init();
    return &GFX_GLOBAL;
}

// Start frame
static Input *gfx_imp_begin(Gfx_Imp *gfx) {
    // Atomic copy of input, js will get events while we are updating
    gfx->input_copy = gfx->input;

    // Reset input for next frame
    input_reset(&gfx->input);

    // Start frame
    gfx_js_begin();
    return &gfx->input_copy;
}

// Grab mouse
static void gfx_set_grab(Gfx *gfx, bool grab) {
    gfx_js_grab(grab);
    gfx->os->input.mouse_is_grabbed = grab;
}

static void gfx_set_fullscreen(Gfx *gfx, bool full) {
    gfx_js_fullscreen(full);
    gfx->os->input.is_fullscreen = full;
}

// Write to texture atlas
static void gfx_imp_texture(Gfx_Imp *gfx, v2u pos, Image *img) {
    gfx_js_texture(pos.x, pos.y, img->size.x, img->size.y, img->pixels);
}

// Perform a draw call
static void gfx_imp_draw(Gfx_Imp *gfx, m44 projection, bool depth, u32 quad_count, Gfx_Quad *quad_list) {
    gfx_js_draw((float *)&projection, depth, quad_count, quad_list);
}

// Finish frame
static void gfx_imp_end(Gfx_Imp *gfx) {
    gfx_js_end();
}

// Callbacks from js
void gfx_js_key_down(u32 key, bool down) {
    // fmt_suu(OS_FMT, "KEY: key=", key, " down=", down, "\n");
    input_emit(&GFX_GLOBAL.input, key_from_char(key), down);
}

void gfx_js_mouse_move(f32 x, f32 y, f32 dx, f32 dy) {
    Input *input = &GFX_GLOBAL.input;
    // fmt_sffff(OS_FMT, "Mouse: x=", x, " y=", y, " dx=", dx, " dy=", dy, "\n");
    input->mouse_moved = true;
    input->mouse_pos.x = x - (f32)input->window_size.x / 2.0;
    input->mouse_pos.y = input->window_size.y / 2.0f - y;
    input->mouse_rel.x += dx;
    input->mouse_rel.y -= dy;
}

void gfx_js_mouse_down(u32 button, bool down) {
    // fmt_suu(OS_FMT, "Mouse: button=", button, " down=", down, "\n");
    if (button == 0) input_emit(&GFX_GLOBAL.input, KEY_MOUSE_LEFT, down);
    if (button == 2) input_emit(&GFX_GLOBAL.input, KEY_MOUSE_RIGHT, down);
}

void gfx_js_resize(i32 width, i32 height) {
    // fmt_suu(OS_FMT, "Resize: width=", width, " height=", height, "\n");
    GFX_GLOBAL.input.window_size.x = width;
    GFX_GLOBAL.input.window_size.y = height;
}

v2 *js_audio_callback(u32 sample_count) {
    assert(sample_count == array_count(GFX_GLOBAL.audio_buffer), "Invalid sample count");
    gfx_audio_callback(sample_count, GFX_GLOBAL.audio_buffer);
    return GFX_GLOBAL.audio_buffer;
}

static void gfx_audio_lock(Gfx *gfx) {
    // No need for locking
}

static void gfx_audio_unlock(Gfx *gfx) {
}
