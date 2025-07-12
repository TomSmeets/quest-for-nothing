// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// gfx.h - A Graphics, Audio, and Input handling abstraction
#pragma once
#include "lib/mem.h"
#include "lib/vec.h"
#include "gfx/input.h"
#include "gfx/image.h"
#include "lib/mat.h"

typedef struct Gfx Gfx;

// Create Window and start playing audio
static Gfx *gfx_init(Memory *mem, const char *title);

// Close window and quit application
static void gfx_quit(Gfx *gfx);

// Start Render
static Input *gfx_begin(Gfx *gfx);

// Finish Render
static void gfx_end(Gfx *gfx);


static void gfx_draw(Gfx *gfx, bool depth, m4 mtx, Image *img);

// Audio Callback that runs in a seperate thread (use locking if needed)
// Fill sample_list with audio samples as they are requested
static void gfx_audio_callback(u32 sample_count, v2 *sample_list);
