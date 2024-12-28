// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// engine.h - Immediate Mode Game Engine
#pragma once

#include "os_gfx.h"

#include "audio.h"
#include "gfx.h"
#include "input.h"
#include "rand.h"
#include "time.h"
#include "ui.h"

typedef struct {
    Memory *tmp;
    Time time;
    OS *os;
    Audio *audio;
    Gfx *gfx;
    Input *input;
    f32 dt;
    Random rng;
    UI ui;

    // X,Y and Z arrows
    Image *image_arrow[3];
} Engine;

static Engine *engine_new(Memory *mem, OS *os, char *title) {
    Engine *eng = mem_struct(mem, Engine);
    eng->os = os;
    eng->audio = audio_new(mem);
    eng->gfx = gfx_new(mem, title);
    eng->input = 0;
    eng->rng = (Random){os_rand()};
    ui_init(&eng->ui, mem, eng->audio, 0, eng->gfx);

    f32 sx = 12 * 2;
    f32 sy = 6 * 2;

    for (u32 j = 0; j < 3; ++j) {
        v3 color = {
            j == 0,
            j == 1,
            j == 2,
        };
        Image *img = image_new(mem, (v2u){sx, sy});
        img->origin = (v2u){0, sy / 2};
        for (u32 i = 0; i <= 3; ++i) {
            image_write(img, (v2i){sx - 1 - i, sy / 2 - i - 1}, color);
            image_write(img, (v2i){sx - 1 - i, sy / 2 + i}, color);
        }
        for (u32 x = 0; x < sx; ++x) {
            image_write(img, (v2i){x, sy / 2 - 1}, color);
            image_write(img, (v2i){x, sy / 2}, color);
        }
        eng->image_arrow[j] = img;
    }
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
