// Game engine core
#pragma once

#include "os_gfx.h"

#include "audio.h"
#include "gfx.h"
#include "time.h"
#include "input.h"


typedef struct {
    Memory *tmp;
    Time time;
    OS *os;
    Audio *audio;
    Gfx   *gfx;
    Input *input;
    f32 dt;
    m4 camera;
} Engine;

static Engine *engine_new(Memory *mem, OS *os, char *title) {
    Engine *eng = mem_struct(mem, Engine);
    eng->os = os;
    eng->audio = audio_new(mem);
    eng->gfx   = gfx_new(mem, title);
    eng->input = 0;
    return eng;
}

static void engine_begin(Engine *eng) {
    eng->dt = time_begin(&eng->time, 120);
    eng->input = gfx_begin(eng->gfx);

    // Allocate memory for this frame (and free at the end of the frame)
    // These memory blocks are reused every frame, so this is very cheap
    eng->tmp = mem_new();
}

static void engine_end(Engine *eng) {
    gfx_end(eng->gfx, eng->camera);
    mem_free(eng->tmp);
    eng->os->sleep_time = time_end(&eng->time);
}
