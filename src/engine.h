// Game engine core
#pragma once

#include "os_gfx.h"

#include "audio.h"
#include "gfx.h"
#include "time.h"
#include "input.h"


typedef struct {
    Audio *audio;
    Gfx   *gfx;
    Input *input;

    m4 camera;
} Engine;

static Engine *engine_new(Memory *mem, char *title) {
    Engine *eng = mem_struct(mem, Engine);
    eng->audio = audio_new(mem);
    eng->gfx   = gfx_new(mem, title);
    eng->input = 0;
    return eng;
}

static void engine_begin(Engine *eng) {
    eng->input = gfx_begin(eng->gfx);
}

static void engine_end(Engine *eng) {
    gfx_end(eng->gfx, eng->camera);
}
