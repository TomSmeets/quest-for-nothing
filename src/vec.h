// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// vec.h - Vector types and linear algebra
#pragma once
#include "types.h"

typedef struct {
    float x, y, z;
} v3;

typedef struct {
    u32 x, y, z;
} v3u;

typedef struct {
    i32 x, y, z;
} v3i;

typedef struct {
    i32 x, y;
} v2i;

typedef struct {
    f32 x, y;
} v2;

typedef struct {
    u32 x, y;
} v2u;

static f32 v3_dot(v3 a, v3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static i32 v3i_dot(v3i a, v3i b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static f32 v3_length_sq(v3 a) {
    return v3_dot(a, a);
}

static i32 v3i_length_sq(v3i a) {
    return v3i_dot(a, a);
}

static v3 v3_add(v3 a, v3 b) {
    return (v3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static v3i v3i_add(v3i a, v3i b) {
    return (v3i){a.x + b.x, a.y + b.y, a.z + b.z};
}

static v3 v3_sub(v3 a, v3 b) {
    return (v3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static v3i v3i_sub(v3i a, v3i b) {
    return (v3i){a.x - b.x, a.y - b.y, a.z - b.z};
}

static f32 v3_distance_sq(v3 a, v3 b) {
    return v3_length_sq(v3_sub(a, b));
}

static i32 v3i_distance_sq(v3i a, v3i b) {
    return v3i_length_sq(v3i_sub(a, b));
}

static bool v3i_eq(v3i a, v3i b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

static v3 v3i_to_v3(v3i v) {
    return (v3){v.x, v.y, v.z};
}
