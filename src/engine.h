// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// engine.h - Immediate Mode Game Engine
#pragma once

#include "audio.h"
#include "gfx.h"
#include "gfx_debug.h"
#include "input.h"
#include "rand.h"
#include "time.h"
#include "ui.h"

typedef struct {
    OS *os;

    // Permanent memory
    Memory *mem;

    // Frame Memory
    Memory *tmp;

    // Frame Timing
    f32 dt;
    Time time;

    // Submodules
    Rand rng;
    Gfx *gfx;
    Gfx_Debug *gfx_dbg;
    Audio *audio;
    Input *input;
    UI *ui;
} Engine;

static Engine *engine_new(Memory *mem, OS *os, Rand rng, char *title) {
    Engine *eng = mem_struct(mem, Engine);
    eng->os = os;
    eng->mem = mem;
    eng->rng = rng;

    eng->gfx = gfx_new(mem, title);
    eng->gfx_dbg = gfx_debug_new(eng->gfx, mem);
    eng->audio = audio_new(mem);
    eng->input = 0;

    eng->ui = ui_new(mem, eng->audio, 0, eng->gfx);
    return eng;
}

static void engine_begin(Engine *eng) {
    // Allocate memory for this frame (and free at the end of the frame)
    // These memory blocks are reused every frame, so this is very cheap
    eng->dt = time_begin(&eng->time, 120);
    eng->tmp = mem_new();
    eng->input = gfx_begin(eng->gfx);
}

static void engine_end(Engine *eng, m4 camera) {
    gfx_end(eng->gfx, camera);
    eng->os->sleep_time = time_end(&eng->time);
    mem_free(eng->tmp);
}
