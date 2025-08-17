// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// engine.h - Immediate Mode Game Engine
#pragma once
#include "gfx/gfx.h"
#include "gfx/input.h"
#include "lib/rand.h"
#include "qfn/gfx_debug.h"
#include "qfn/time.h"
#include "qfn/ui.h"

typedef struct {
    // Permanent memory
    Memory *mem;

    // Frame Memory
    Memory *tmp;

    // Frame Timing
    f32 dt;

    // Submodules
    Rand rng;
    Gfx *gfx;
    Gfx_Debug *gfx_dbg;
    Input *input;
    UI *ui;
} Engine;

static Engine *engine_new(Memory *mem, Rand rng, char *title) {
    Engine *eng = mem_struct(mem, Engine);
    eng->mem = mem;
    eng->rng = rng;

    eng->gfx = gfx_init(mem, title);
    eng->gfx_dbg = gfx_debug_new(eng->gfx, mem);
    eng->input = 0;

    eng->ui = ui_new(mem, 0, eng->gfx);
    return eng;
}

static void engine_begin(Engine *eng) {
    // Allocate memory for this frame (and free at the end of the frame)
    // These memory blocks are reused every frame, so this is very cheap
    eng->dt = G->dt;
    eng->tmp = mem_new();
    eng->input = gfx_begin(eng->gfx);
}

static void engine_end(Engine *eng, m4 camera) {
    gfx_end(eng->gfx, camera);
    mem_free(eng->tmp);
}
