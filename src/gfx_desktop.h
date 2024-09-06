#pragma once
#include "gfx_api.h"
#include "mem.h"
#include "ogl.h"
#include "os.h"
#include "sdl.h"

struct Gfx {
    Memory *mem;
    Sdl *sdl;
    OGL *ogl;
};

static Gfx *os_gfx_init(Memory *mem, char *title) {
    Gfx *gfx = mem_struct(mem, Gfx);
    void *handle = os_dlopen(OS_IS_LINUX ? "libSDL2.so" : "SDL2.dll");
    gfx->mem = mem;
    gfx->sdl = sdl_load(mem, handle, title);
    gfx->ogl = ogl_load(mem, gfx->sdl->api.SDL_GL_GetProcAddress);
    return gfx;
}

static Input *os_gfx_poll(Gfx *gfx) {
    return sdl_poll(gfx->sdl);
}

static void os_gfx_set_mouse_grab(Gfx *gfx, bool grab) {
    sdl_set_mouse_grab(gfx->sdl, grab);
}

static void os_gfx_begin(Gfx *gfx, m4s *proj) {
    // If we want to reload opengl shaders, etc
    if (OS_GLOBAL->reloaded) {
        gfx->ogl = ogl_load(gfx->mem, gfx->sdl->api.SDL_GL_GetProcAddress);
    }

    ogl_begin(gfx->ogl, proj, gfx->sdl->input.window_size);
}

static void os_gfx_quad(Gfx *gfx, m4s *mtx, Image *img) {
    ogl_quad(gfx->ogl, mtx, img);
}

static void os_gfx_end(Gfx *gfx) {
    ogl_draw(gfx->ogl);
    sdl_swap_window(gfx->sdl);
}
