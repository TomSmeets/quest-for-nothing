// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// mat.h - Math for 4d matrices on homogeneous coordinates
#pragma once
#include "vec.h"

// Single 4x4 matrix
// column major, just like opengl
// This allows us to extract the columns directly
// | X.x   Y.x   Z.x   W.x |
// |                       |
// | X.y   Y.y   Z.y   W.y |
// |                       |
// | X.z   Y.z   Z.z   W.z |
// |                       |
// | X.w   Y.w   Z.w   W.w |
typedef struct {
    v4 x;
    v4 y;
    v4 z;
    v4 w;
} m4s;

// Multiply a 4 component vector with this matrix
// y = M * x
static v4 m4s_mul_v4(m4s *m, v4 v) {
    return v.x * m->x + v.y * m->y + v.z * m->z + v.w * m->w;
}

// Transform a position by the transformation matrix
static v3 m4s_mul_pos(m4s *m, v3 v) {
    return (v.x * m->x + v.y * m->y + v.z * m->z + m->w).xyz;
}

// Transform a direction by the transformation matrix
static v3 m4s_mul_dir(m4s *m, v3 v) {
    return (v.x * m->x + v.y * m->y + v.z * m->z).xyz;
}

// C = A * B
static m4s m4s_mul(m4s *a, m4s *b) {
    return (m4s){
        .x = m4s_mul_v4(a, b->x),
        .y = m4s_mul_v4(a, b->y),
        .z = m4s_mul_v4(a, b->z),
        .w = m4s_mul_v4(a, b->w),
    };
}

static m4s m4s_id(void) {
    return (m4s){
        .x = {1, 0, 0, 0},
        .y = {0, 1, 0, 0},
        .z = {0, 0, 1, 0},
        .w = {0, 0, 0, 1},
    };
}

static m4s m4s_scale(v3 s) {
    return (m4s){
        .x = {s.x, 0, 0, 0},
        .y = {0, s.y, 0, 0},
        .z = {0, 0, s.z, 0},
        .w = {0, 0, 0, 1},
    };
}

static m4s m4s_trans(v3 t) {
    return (m4s){
        .x = {1, 0, 0, 0},
        .y = {0, 1, 0, 0},
        .z = {0, 0, 1, 0},
        .w = {t.x, t.y, t.z, 1},
    };
}
