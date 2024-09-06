#pragma once

#include "os_base.h"
#include "vec.h"
#include "input.h"
#include "math.h"
#include "image.h"

typedef struct Gfx Gfx;

// Callback
static void os_audio_callback(OS *os, u32 count, v2 *samples);

static Gfx *os_gfx_init(Memory *mem, char *title);

static Input *os_gfx_begin(Gfx *gfx);
static void os_gfx_quad(Gfx *gfx, m4s mtx, Image *img);
static void os_gfx_end(Gfx *gfx);
