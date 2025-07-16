#pragma once
#include "gfx/gfx2.h"
#include "gfx/gfx2_help.h"
#include "lib/os_api_wasm.h"

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

WASM_IMPORT(wasm_gfx_begin) void wasm_gfx_begin(void);
static Input *gfx_begin(Gfx *gfx) {
    // Double buffer input because we can recieve input callbacks at any time
    gfx->input = gfx->next_input;
    input_reset(&gfx->next_input);
    wasm_gfx_begin();
    return &gfx->input;
}

WASM_IMPORT(wasm_gfx_texture) void wasm_gfx_texture(u32 x, u32 y, u32 sx, u32 sy, void *pixels);
WASM_IMPORT(wasm_gfx_clear) void wasm_gfx_clear(void);
WASM_IMPORT(wasm_gfx_draw) void wasm_gfx_draw(float *projection, bool depth, u32 quad_count, Gfx_Quad *quad_list);
WASM_IMPORT(wasm_gfx_end) void wasm_gfx_end(void);
static void gfx_end(Gfx *gfx, m4 camera) {

    v2 aspect = ogl_aspect(gfx->input.window_size);
    m4 view = m4_invert_tr(camera);
    m44 projection = m4_perspective_to_clip(view, 70, aspect.x, aspect.y, 0.1, 15.0);

    // Graphics
    wasm_gfx_clear();
    wasm_gfx_begin_ui();
    gl->glViewport(0, 0, gfx->input.window_size.x, gfx->input.window_size.y);
    gl->glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    gl->glBindBuffer(GL_ARRAY_BUFFER, gfx->instance_buffer);
    gl->glUniformMatrix4fv(gfx->uniform_proj, 1, false, (GLfloat *)&projection);

    gl->glEnable(GL_DEPTH_TEST);
    gl->glDisable(GL_BLEND);
    gfx_draw_pass(gfx, gfx->help.pass_3d);

    gl->glDisable(GL_DEPTH_TEST);
    gl->glEnable(GL_BLEND);
    gl->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    gfx_draw_pass(gfx, gfx->help.pass_ui);

    // Swap
    sdl->SDL_GL_SwapWindow(gfx->window);

    wasm_gfx_clear();
    wasm_gfx_end();
}

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
