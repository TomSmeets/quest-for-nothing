// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// color.h - Color types and math
#pragma once
#include "lib/vec.h"

#define COLOR_WHITE ((v3){1, 1, 1})
#define COLOR_BLACK ((v3){0, 0, 0})
#define COLOR_GRAY ((v3){0.5, 0.5, 0.5})
#define COLOR_RED ((v3){1, 0, 0})
#define COLOR_GREEN ((v3){0, 1, 0})
#define COLOR_BLUE ((v3){0, 0, 1})

static v3 color_blend(v3 a, v3 b, f32 alpha) {
    return a * (1 - alpha) + b * alpha;
}

static v4 color_alpha(v3 color, f32 alpha) {
    return (v4){color.x, color.y, color.z, alpha};
}

static v4 color4(v3 color) {
    return (v4){color.x, color.y, color.z, 1};
}

static v3 color_rainbow(f32 v) {
    f32 r = 0.5f + 0.5 * f_cos2pi(v + 0.0f / 3.0f);
    f32 g = 0.5f + 0.5 * f_cos2pi(v + 1.0f / 3.0f);
    f32 b = 0.5f + 0.5 * f_cos2pi(v + 2.0f / 3.0f);
    return (v3){r, g, b};
}
