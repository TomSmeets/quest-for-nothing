#pragma once
#include "gfx_api.h"
#include "os.h"

WASM_IMPORT(js_gfx_grab) void js_gfx_grab(bool grab);

struct Gfx {
    Input input;
};

static Gfx *os_gfx_init(Memory *mem, char *title) {
    Gfx *gfx = mem_struct(mem, Gfx);
    return gfx;
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
