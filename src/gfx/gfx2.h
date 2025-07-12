// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// gfx.h - A Graphics, Audio, and Input handling abstraction
#pragma once
#include "gfx/image.h"
#include "gfx/input.h"
#include "lib/mat.h"
#include "lib/mem.h"
#include "lib/vec.h"

#define GFX_AUDIO_RATE 48000

typedef struct Gfx Gfx;

// Audio Callback that runs in a seperate thread (use locking if needed)
// Fill sample_list with audio samples as they are requested
static void gfx_audio_callback(u32 sample_count, v2 *sample_list);

// Create Window and start playing audio
static Gfx *gfx_init(Memory *mem, const char *title);

// Close window and quit application
static void gfx_quit(Gfx *gfx);

// Start Render
static Input *gfx_begin(Gfx *gfx);

// Finish Render
static void gfx_end(Gfx *gfx, m4 camera);

// Draw image during render
static void gfx_draw(Gfx *gfx, bool depth, m4 mtx, Image *img);

// Set mouse grab
static void gfx_set_grab(Gfx *gfx, bool grab);

// Set Window fullscreen
static void gfx_set_fullscreen(Gfx *gfx, bool fullscreen);
