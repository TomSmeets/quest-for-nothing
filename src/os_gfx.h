// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_gfx.h - Graphics, Audio, and Input handling abstraction api definition
#pragma once
#include "asset.h"
#include "image.h"
#include "input.h"
#include "mat.h"
#include "math.h"
#include "os.h"
#include "vec.h"

// Size of the square texture atlas
#define OS_GFX_ATLAS_SIZE 4096
#define AUDIO_SAMPLE_RATE 48000

typedef struct OS_Gfx OS_Gfx;

typedef struct {
    f32 x[3];
    f32 y[3];
    f32 z[3];
    f32 w[3];
    f32 uv_pos[2];
    f32 uv_size[2];
} OS_Gfx_Quad;

static_assert(sizeof(OS_Gfx_Quad) == 4 * 16);

// Initialize Graphics stack
static OS_Gfx *os_gfx_init(Memory *mem, char *title);

// Start frame
static Input *os_gfx_begin(OS_Gfx *gfx);

// Grab mouse
static void os_gfx_set_grab(OS_Gfx *gfx, bool grab);
static void os_gfx_set_fullscreen(OS_Gfx *gfx, bool full);

// Write to texture atlas
static void os_gfx_texture(OS_Gfx *gfx, v2u pos, Image *img);

// Perform a draw call
static void os_gfx_draw(OS_Gfx *gfx, m44 projection, bool depth, u32 quad_count, OS_Gfx_Quad *quad_list);

// Audio callback, called from platform layer
static void os_gfx_audio_callback(u32 count, v2 *samples);

// Finish frame
static void os_gfx_end(OS_Gfx *gfx);

#if OS_IS_WINDOWS || OS_IS_LINUX
#include "gfx_desktop.h"
#elif OS_IS_WASM
#include "gfx_wasm.h"
#include "os_wasm.h"
#endif
