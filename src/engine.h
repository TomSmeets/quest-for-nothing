// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// engine.h - Immediate Mode Game Engine
#pragma once

#include "os_gfx.h"

#include "audio.h"
#include "gfx.h"
#include "input.h"
#include "rand.h"
#include "time.h"

typedef struct {
    Memory *tmp;
    Time time;
    OS *os;
    Audio *audio;
    Gfx *gfx;
    Input *input;
    f32 dt;
    Random rng;

    Image *image_arrow;
} Engine;

static Engine *engine_new(Memory *mem, OS *os, char *title) {
    Engine *eng = mem_struct(mem, Engine);
    eng->os = os;
    eng->audio = audio_new(mem);
    eng->gfx = gfx_new(mem, title);
    eng->input = 0;
    eng->rng = (Random){os_rand()};

    eng->image_arrow = image_new(mem, (v2u){10, 5});
    image_write(eng->image_arrow, (v2i){9 - 1, 1}, RED);
    image_write(eng->image_arrow, (v2i){9 - 2, 0}, RED);
    image_write(eng->image_arrow, (v2i){9 - 1, 3}, RED);
    image_write(eng->image_arrow, (v2i){9 - 2, 4}, RED);
    for (u32 i = 0; i < 10; ++i) image_write(eng->image_arrow, (v2i){i, 2}, RED);

    return eng;
}

static void engine_begin(Engine *eng) {
    eng->dt = time_begin(&eng->time, 120);
    eng->input = gfx_begin(eng->gfx);

    // Allocate memory for this frame (and free at the end of the frame)
    // These memory blocks are reused every frame, so this is very cheap
    eng->tmp = mem_new();
}

static void engine_end(Engine *eng, m4 camera) {
    gfx_end(eng->gfx, camera);
    mem_free(eng->tmp);
    eng->os->sleep_time = time_end(&eng->time);
}
