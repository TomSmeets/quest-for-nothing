// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// vec.h - Vector Math v2
#pragma once
#include "math.h"

// TODO: how much do I actually use this? Can we do without vector extensions?
// NOTE: maybe we just always use v3 for everything? That way we don't need any conversions.
// A 2d vector using clang vector extensions
typedef f32 v2 __attribute__((ext_vector_type(2)));
typedef f32 v3 __attribute__((ext_vector_type(3)));
typedef f32 v4 __attribute__((ext_vector_type(4)));

typedef f32 v2i __attribute__((ext_vector_type(2)));

// Singed angle to the +x axis [-Pi, Pi]
static f32 v2_angle(v2 v) {
    return f_atan2(v.y, v.x);
}


// Vector dot product
static f32 v2_dot(v2 a, v2 b) { return a.x*b.x + a.y*b.y;           }
static f32 v3_dot(v3 a, v3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }

// |v|^2
static f32 v2_len_squared(v2 v) { return v2_dot(v, v); }
static f32 v3_len_squared(v3 v)  { return v3_dot(v, v); }

// |v|
static f32 v2_len(v2 v)  { return f_sqrt(v2_len_squared(v)); }
static f32 v3_len(v3 v)  { return f_sqrt(v3_len_squared(v)); }

// Normalize a vector
// returns 0 for a zero vector
static v2 v2_normalize(v2 v)  { return v * f_inv_sqrt(v2_len_squared(v)); }
static v3 v3_normalize(v3 v)  { return v * f_inv_sqrt(v3_len_squared(v)); }

// Rotate vector by 90* (Counter Clockwise)
static v2 v2_rot90(v2 v) { return (v2) { -v.y, v.x }; };

// Limit the length of a vector to some maximum
static v3 v3_limit(v3 v, f32 lim) {
    f32 len2 = v3_len_squared(v);
    if(len2 <= lim*lim) return v;
    return v * lim * f_inv_sqrt(len2);
}
