// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// gfx_deskitop.h - Gfx implentation for WASM
#pragma once
#include "gfx/gfx.h"
#include "gfx/gfx_help.h"
#include "gfx/ogl.h"
#include "lib/fmt.h"
#include "lib/os_api_wasm.h"

struct Gfx {
    Input input;
    Input next_input;
    v2 sample_buffer[1024];

    Memory *tmp;
    Packer *pack;
    Gfx_Pass *pass_3d;
    Gfx_Pass *pass_ui;
    Gfx_Pass_Compiled result;
};

struct Gfx GFX_GLOBAL;

WASM_IMPORT(wasm_gfx_init) void wasm_gfx_init(void);
static Gfx *gfx_init(Memory *mem, const char *title) {
    Gfx *gfx = &GFX_GLOBAL;
    fmt_s(G->fmt, "gfx_init\n");
    wasm_gfx_init();
    return gfx;
}

WASM_IMPORT(wasm_gfx_begin_audio) void wasm_gfx_begin_audio(void);
static Input *gfx_begin(Gfx *gfx) {
    fmt_s(G->fmt, "gfx_begin\n");
    // Double buffer input because we can recieve input callbacks at any time
    gfx->input = gfx->next_input;
    input_reset(&gfx->next_input);
    wasm_gfx_begin_audio();

    gfx->tmp = mem_new();
    gfx->pass_3d = 0;
    gfx->pass_ui = 0;
    return &gfx->input;
}

WASM_IMPORT(wasm_gfx_texture) void wasm_gfx_texture(u32 x, u32 y, u32 sx, u32 sy, void *pixels);
WASM_IMPORT(wasm_gfx_draw) void wasm_gfx_draw(u32 quad_count, Gfx_Quad *quad_list);
static void gfx_draw_pass(Gfx *gfx, Gfx_Pass *pass) {
    Gfx_Pass_Compiled *result = &gfx->result;
    while (gfx_pass_compile(result, &gfx->pack, &pass)) {
        for (u32 i = 0; i < result->upload_count; ++i) {
            Gfx_Upload *upload = result->upload_list + i;
            u32 x = upload->pos.x;
            u32 y = upload->pos.y;
            u32 w = upload->size.x;
            u32 h = upload->size.y;
            wasm_gfx_texture(x, y, w, h, upload->pixels);
        }
        wasm_gfx_draw(result->quad_count, result->quad_list);
    }
}

WASM_IMPORT(wasm_gfx_clear) void wasm_gfx_clear(void);
WASM_IMPORT(wasm_gfx_begin_3d) void wasm_gfx_begin_3d(m44 *projection);
WASM_IMPORT(wasm_gfx_begin_ui) void wasm_gfx_begin_ui(m44 *projection);
static void gfx_end(Gfx *gfx, m4 camera) {
    fmt_s(G->fmt, "gfx_end\n");
    v2 aspect = ogl_aspect(gfx->input.window_size);
    m4 view = m4_invert_tr(camera);
    m44 projection = m4_perspective_to_clip(view, 70, aspect.x, aspect.y, 0.1, 15.0);

    // Graphics
    wasm_gfx_clear();
    wasm_gfx_begin_3d(&projection);
    gfx_draw_pass(gfx, gfx->pass_3d);
    wasm_gfx_begin_ui(&projection);
    gfx_draw_pass(gfx, gfx->pass_ui);
    mem_free(gfx->tmp);
    gfx->tmp = 0;
}

static void gfx_draw_3d(Gfx *gfx, m4 mtx, Image *img) {
    gfx_pass_push(gfx->tmp, &gfx->pass_3d, mtx, img);
}

static void gfx_draw_ui(Gfx *gfx, m4 mtx, Image *img) {
    gfx_pass_push(gfx->tmp, &gfx->pass_ui, mtx, img);
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
    if (button == 0) input_emit(&GFX_GLOBAL.next_input, KEY_MOUSE_LEFT, down);
    if (button == 2) input_emit(&GFX_GLOBAL.next_input, KEY_MOUSE_RIGHT, down);
}

void wasm_gfx_resize(i32 width, i32 height) {
    GFX_GLOBAL.next_input.window_size.x = width;
    GFX_GLOBAL.next_input.window_size.y = height;
}

// Called from JS
v2 *wasm_gfx_audio_callback(u32 sample_count) {
    assert(sample_count == array_count(GFX_GLOBAL.sample_buffer), "Invalid sample count");
    gfx_audio_callback(sample_count, GFX_GLOBAL.sample_buffer);
    return GFX_GLOBAL.sample_buffer;
}
