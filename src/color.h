// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// color.h - Color types and math
#pragma once
#include "vec.h"

#define WHITE ((v3){1, 1, 1})
#define BLACK ((v3){0, 0, 0})
#define GRAY ((v3){0.5, 0.5, 0.5})
#define RED ((v3){1, 0, 0})
#define GREEN ((v3){0, 1, 0})
#define BLUE ((v3){0, 0, 1})

static v3 color_blend(v3 a, v3 b, f32 alpha) {
    return a * (1 - alpha) + b * alpha;
}

static v4 color_alpha(v3 color, f32 alpha) {
    return (v4){color.x, color.y, color.z, alpha};
}
