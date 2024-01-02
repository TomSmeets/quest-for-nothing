// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// color.h - Methods related to operations on color
#pragma once
#include "vec.h"

#define WHITE ((v4){1, 1, 1, 1})
#define BLACK ((v4){0, 0, 0, 1})
#define RED   ((v4){1, 0, 0, 1})
#define GREEN ((v4){0, 1, 0, 1})
#define BLUE  ((v4){0, 0, 1, 1})
#define GRAY  ((v4){0.8, .8, .8, 1})

// All color is always stored in linear space
static u32 col_to_u32(v4 c) {
    u32 v = 0;
    v = (v << 8) | (u8)(c.w * 255); // A
    v = (v << 8) | (u8)(c.z * 255); // B
    v = (v << 8) | (u8)(c.y * 255); // G
    v = (v << 8) | (u8)(c.x * 255); // R
    return v;
}

static v4 u32_to_col(u32 v) {
    f32 r = (f32)((v >> (8 * 0)) & 0xff) / 255.f;
    f32 g = (f32)((v >> (8 * 1)) & 0xff) / 255.f;
    f32 b = (f32)((v >> (8 * 2)) & 0xff) / 255.f;
    f32 a = (f32)((v >> (8 * 3)) & 0xff) / 255.f;
    return (v4){r, g, b, a};
}

static v4 col_rainbow(f32 v) {
    // we use half angle, because we square later
    f32 r = f_cos((v + 0 / 3.0f) * R2);
    f32 g = f_cos((v + 1 / 3.0f) * R2);
    f32 b = f_cos((v + 2 / 3.0f) * R2);
    r *= r;
    g *= g;
    b *= b;
    return (v4){r, g, b, 1.0};
}
