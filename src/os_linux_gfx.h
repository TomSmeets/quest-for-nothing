#pragma once
#include "os_base.h"
#include "mem.h"
#include "sdl.h"
#include "ogl.h"

typedef struct {
    Memory *mem;
    Sdl *sdl;
    OGL *ogl;
} Gfx;

static Gfx *os_gfx_init(Memory *mem, char *title) {
    Gfx *gfx = mem_struct(mem, Gfx);
    void *handle = os_dlopen("libSDL2.so");
    gfx->sdl = sdl_load(mem, handle, title);
    gfx->ogl = ogl_load(mem, gfx->sdl->api.SDL_GL_GetProcAddress);
    return gfx;
}



// Todo, top level shouold be os here. remove this call
static void *os_load_sdl2(char *name) {
    OS *os = OS_GLOBAL;

    if (!os->sdl2_handle) {
        os->sdl2_handle = dlopen("libSDL2.so", RTLD_LOCAL | RTLD_NOW);
    }

    return dlsym(os->sdl2_handle, name);
}
