// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// vec.h - Vector types and linear algebra
#pragma once
#include "lib/math.h"
#include "lib/types.h"

typedef f32 v2 __attribute__((ext_vector_type(2)));
typedef f32 v3 __attribute__((ext_vector_type(3)));
typedef f32 v4 __attribute__((ext_vector_type(4)));

typedef u32 v2u __attribute__((ext_vector_type(2)));
typedef u32 v3u __attribute__((ext_vector_type(3)));
typedef u32 v4u __attribute__((ext_vector_type(4)));

typedef i32 v2i __attribute__((ext_vector_type(2)));
typedef i32 v3i __attribute__((ext_vector_type(3)));
typedef i32 v4i __attribute__((ext_vector_type(4)));

static_assert(sizeof(v2) == 2 * 4);
static_assert(sizeof(v3) == 4 * 4);
static_assert(sizeof(v4) == 4 * 4);

static_assert(sizeof(v2i) == 2 * 4);
static_assert(sizeof(v3i) == 4 * 4);
static_assert(sizeof(v4i) == 4 * 4);

static_assert(sizeof(v2u) == 2 * 4);
static_assert(sizeof(v3u) == 4 * 4);
static_assert(sizeof(v4u) == 4 * 4);

// clang-format off
// Basics
static bool v3i_eq(v3i a, v3i b) { return a.x == b.x && a.y == b.y && a.z == b.z; }

// Math
static v2 v2_rot90(v2 v) { return (v2){-v.y, v.x}; };
static v2 v2_from_rot(f32 a) { return (v2) { f_cos(a), f_sin(a) }; }

static f32 v2_dot(v2 a, v2 b)    { return a.x * b.x + a.y * b.y; }
static f32 v3_dot(v3 a, v3 b)    { return a.x * b.x + a.y * b.y + a.z * b.z; }
static i32 v3i_dot(v3i a, v3i b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

static f32 v2_length_sq(v2 a)    { return v2_dot(a, a); }
static f32 v2_length(v2 a)    { return f_sqrt(v2_length_sq(a)); }
static f32 v3_length_sq(v3 a)    { return v3_dot(a, a); }
static f32 v3_length(v3 a)    { return f_sqrt(v3_length_sq(a)); }
static i32 v3i_length_sq(v3i a)  { return v3i_dot(a, a); }

static f32 v3_distance_sq(v3 a, v3 b)    { return v3_length_sq(a - b); }
static i32 v3i_distance_sq(v3i a, v3i b) { return v3i_length_sq(a - b); }

static v2 v2_normalize(v2 v) { return v * f_inv_sqrt(v2_length_sq(v)); }
static v3 v3_normalize(v3 v) { return v * f_inv_sqrt(v3_length_sq(v)); }

// Limit the length of a vector to some maximum
static v3 v3_limit(v3 v, f32 low, f32 lim) {
    f32 len2 = v3_length_sq(v);

    if (len2 > lim * lim)
        return v * lim * f_inv_sqrt(len2);

    if (len2 < low * low)
        return 0;

    return v;
}

// Limit the length of a vector to some maximum
static v2 v2_limit(v2 v, f32 low, f32 lim) {
    f32 len2 = v2_length_sq(v);

    if (len2 > lim * lim)
        return v * lim * f_inv_sqrt(len2);

    if(len2 < low*low)
        return 0;

    return v;
}

// Casting
static v3 v3i_to_v3(v3i v) { return (v3){v.x, v.y, v.z}; }
static v3i v3_to_v3i(v3 v) { return (v3i){v.x, v.y, v.z}; }
static v2 v2i_to_v2(v2i v) { return (v2){v.x, v.y}; }
static v2 v2u_to_v2(v2u v) { return (v2){v.x, v.y}; }

// clang-format on

// 3d Vecotro Cross product
// cross(x,y) = z
// cross(y,z) = x
// cross(z,x) = y
static v3 v3_cross(v3 a, v3 b) {
    v3 res;
    res.x = a.y * b.z - a.z * b.y;
    res.y = a.z * b.x - a.x * b.z;
    res.z = a.x * b.y - a.y * b.x;
    return res;
}
