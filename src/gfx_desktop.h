// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gfx_desktop.h - Gfx implementation for windows and linux using SDL2
#pragma once
#include "gfx_api.h"
#include "mem.h"
#include "ogl.h"
#include "sdl.h"

struct Gfx {
    Memory *mem;
    Sdl *sdl;
    OGL *ogl;
};

static Gfx *os_gfx_init(Memory *mem, char *title) {
    // Gfx structure stores handles to SDL and OpenGL
    Gfx *gfx = mem_struct(mem, Gfx);
    gfx->mem = mem;

    // Load SDL2 Library
    File *lib = os_dlopen(OS_IS_LINUX ? "libSDL2.so" : "SDL2.dll");
    gfx->sdl = sdl_load(mem, lib, title);

    // Load OpenGL library
    gfx->ogl = ogl_load(mem, gfx->sdl->api.SDL_GL_GetProcAddress);
    return gfx;
}

// Poll SDL2 input and window events and combine them
// into a Input structure
static Input *os_gfx_poll(Gfx *gfx) {
    return sdl_poll(gfx->sdl);
}

// Grab or release the mouse
//   Grabbed -> Relative mouse coordinates
//   Released -> Absolute mouse coordinates
static void os_gfx_set_mouse_grab(Gfx *gfx, bool grab) {
    sdl_set_mouse_grab(gfx->sdl, grab);
}

// Begin a new graphics frame
// call os_gfx_quad to draw quads
// call os_gfx_end to finish this frame
static void os_gfx_begin(Gfx *gfx) {
    // If we want to reload opengl shaders during a hot-reload
    if (1 && OS_GLOBAL->reloaded) {
        gfx->ogl = ogl_load(gfx->mem, gfx->sdl->api.SDL_GL_GetProcAddress);
    }

    ogl_begin(gfx->ogl, gfx->sdl->input.window_size);
}

// Draw a single textured quad in the given scale and orientation
static void os_gfx_quad(Gfx *gfx, m4 mtx, Image *img, bool ui) {
    ogl_quad(gfx->ogl, mtx, img, ui);
}

// Finish drawing the frame and submit it to the gpu
static void os_gfx_end(Gfx *gfx, m4 camera) {
    ogl_draw(gfx->ogl, camera, gfx->sdl->input.window_size);
    sdl_swap_window(gfx->sdl);
}
