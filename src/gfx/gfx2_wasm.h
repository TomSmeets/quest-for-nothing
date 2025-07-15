#pragma once
#include "gfx/gfx2.h"
#include "lib/os_api_wasm.h"

WASM_IMPORT(wasm_gfx_quit)    void wasm_gfx_quit(void);
WASM_IMPORT(wasm_gfx_begin) void wasm_gfx_begin(void);
WASM_IMPORT(wasm_gfx_end) void wasm_gfx_end(void);

// Helpers
WASM_IMPORT(wasm_gfx_draw)    void wasm_gfx_draw(float *projection, bool depth, u32 quad_count, Gfx_Quad *quad_list);
WASM_IMPORT(wasm_gfx_texture) void wasm_gfx_texture(u32 x, u32 y, u32 sx, u32 sy, void *pixels);

struct Gfx {
    Input input;
    Input next_input;
    v2 sample_buffer[1024];
};

struct Gfx GFX_GLOBAL;

WASM_IMPORT(wasm_gfx_init) void wasm_gfx_init(void);
static Gfx *gfx_init(Memory *mem, const char *title) {
    Gfx *gfx = &GFX_GLOBAL;
    wasm_gfx_init();
    return gfx;
}

static void gfx_quit(Gfx *gfx) {}

static Input *gfx_begin(Gfx *gfx) {
    gfx->input = gfx->next_input;
    input_reset(&gfx->next_input);
    return &gfx->input;
}

static void gfx_end(Gfx *gfx, m4 camera) {}

static void gfx_draw(Gfx *gfx, bool depth, m4 mtx, Image *img) {
}

WASM_IMPORT(wasm_gfx_set_grab) void wasm_gfx_set_grab(bool grab);
static void gfx_set_grab(Gfx *gfx, bool grab) {
    gfx->input.mouse_is_grabbed = grab;
    wasm_gfx_set_grab(grab);
}

// Set Window fullscreen
WASM_IMPORT(wasm_gfx_set_fullscreen) void wasm_gfx_set_fullscreen(bool fullscreen);
static void gfx_set_fullscreen(Gfx *gfx, bool fullscreen) {
    gfx->input.is_fullscreen = fullscreen;
    wasm_gfx_set_fullscreen(fullscreen);
}

// Callbacks from js
void wasm_gfx_key_down(u32 key, bool down) {
    input_emit(&GFX_GLOBAL.next_input, key_from_char(key), down);
}

void wasm_gfx_mouse_move(f32 x, f32 y, f32 dx, f32 dy) {
    Input *input = &GFX_GLOBAL.next_input;
    input->mouse_moved = true;
    input->mouse_pos.x = x - (f32)input->window_size.x / 2.0;
    input->mouse_pos.y = input->window_size.y / 2.0f - y;
    input->mouse_rel.x += dx;
    input->mouse_rel.y -= dy;
}

void wasm_gfx_mouse_down(u32 button, bool down) {
    // fmt_suu(G->fmt, "Mouse: button=", button, " down=", down, "\n");
    if (button == 0) input_emit(&GFX_GLOBAL.next_input, KEY_MOUSE_LEFT, down);
    if (button == 2) input_emit(&GFX_GLOBAL.next_input, KEY_MOUSE_RIGHT, down);
}

void wasm_gfx_resize(i32 width, i32 height) {
    // fmt_suu(G->fmt, "Resize: width=", width, " height=", height, "\n");
    GFX_GLOBAL.next_input.window_size.x = width;
    GFX_GLOBAL.next_input.window_size.y = height;
}

// Called from JS
v2 *wasm_gfx_audio_callback(u32 sample_count) {
    assert(sample_count == array_count(GFX_GLOBAL.sample_buffer), "Invalid sample count");
    gfx_audio_callback(sample_count, GFX_GLOBAL.sample_buffer);
    return GFX_GLOBAL.sample_buffer;
}

