// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gfx_api.h - Graphics, Audio, and Input handling abstraction api definition
#pragma once
#include "image.h"
#include "input.h"
#include "mat.h"
#include "math.h"
#include "os.h"
#include "vec.h"

typedef struct Gfx Gfx;

// Callback
static void os_audio_callback(OS *os, f32 dt, u32 count, v2 *samples);

static Gfx *os_gfx_init(Memory *mem, char *title);

static Input *os_gfx_poll(Gfx *gfx);
static void os_gfx_set_mouse_grab(Gfx *gfx, bool grab);

static void os_gfx_begin(Gfx *gfx);
static void os_gfx_quad(Gfx *gfx, m4s *mtx, Image *img, bool ui);
static void os_gfx_end(Gfx *gfx, m4s *projection, m4s *screen);
