#pragma once
#include "gfx.h"

typedef struct {
    f32 x[3];
    f32 y[3];
    f32 z[3];
    f32 w[3];
    f32 uv_pos[2];
    f32 uv_size[2];
} OGL_Quad;

static_assert(sizeof(OGL_Quad) == 64);

typedef struct {
} Gfx;

