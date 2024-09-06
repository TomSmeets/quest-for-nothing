#pragma once

#include "os_base.h"
#include "vec.h"
#include "input.h"
#include "math.h"

static void os_audio_callback(OS *os, u32 count, v2 *samples);

static void os_gfx_init(char *title);
static Input *os_gfx_begin(void);
static void os_gfx_quad(m4s mtx);
static void os_gfx_end(void);
