// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// mat.h - Math for 4d matrices on homogeneous coordinates (including the matrix inverse)
#pragma once
#include "mat_single.h"
#include "vec.h"

// Double 4x4 matrix containing both the forward and inverse matrix
typedef struct {
    m4s fwd;
    m4s inv;
} m4;

typedef struct {
    f32 v[4][4];
}m44;

// Identity matrix, does nothing
static m4 m4_id(void) {
    m4s id = m4s_id();
    return (m4){id, id};
}

// Invert the matrix.
// This is a cheap operation in this case because we already computed the inverse
static void m4_inv(m4 *m) {
    *m = (m4){m->inv, m->fwd};
}

// left multiply by B
// M = B * M
static void m4_mul(m4 *m, m4 *b) {
    m->fwd = m4s_mul(&b->fwd, &m->fwd);
    m->inv = m4s_mul(&m->inv, &b->inv);
}

// righ multiply by B
// M = M * B
static void m4_mul_right(m4 *m, m4 *b) {
    m->fwd = m4s_mul(&m->fwd, &b->fwd);
    m->inv = m4s_mul(&b->inv, &m->inv);
}

// left multiply by b inverse
// m = 1/b * m
static void m4_mul_inv(m4 *m, m4 *b) {
    m->fwd = m4s_mul(&b->inv, &m->fwd);
    m->inv = m4s_mul(&m->inv, &b->fwd);
}

// Scale on the x, y, and z axis
// m = S * m
static void m4_scale(m4 *m, v3 s) {
    m4 a = {
        .fwd = m4s_scale(s),
        .inv = m4s_scale(1.0 / s),
    };
    m4_mul(m, &a);
}

// Translate the matrix
// m = T * m
static void m4_trans(m4 *m, v3 t) {
    m4 a = {
        .fwd = m4s_trans(t),
        .inv = m4s_trans(-t),
    };
    m4_mul(m, &a);
}



// Compute the perspective projection matrix
// x: Left   --> Right
// y: Bottom --> Top
// z: Far    --> Near
// See: https://www.youtube.com/watch?v=U0_ONQQ5ZNM
//
// OpenGL: (x,y,z,w) -> (x/w, y/w, z/w)
//
// So we want (x, y, z^2, z)
// But that is not possible.
// However we can solve the equation for near and far.

static m44 m4_perspective_to_clip(m4s mtx, f32 vertical_fov, f32 aspect_w_over_h, f32 near_v, f32 far_v) {
    f32 tan_vertical_fov = f_tan(vertical_fov * DEG_TO_RAD * 0.5f);

    f32 a = aspect_w_over_h * tan_vertical_fov;
    f32 b = tan_vertical_fov;
    f32 d = far_v / (far_v - near_v);

    // Perspective Projection Matrix
    // Maps 3D -> 2D
    f32 sx = -1.0f / a;
    f32 sy = -1.0f / b;
    f32 sz = d;

    // mul
    // P * M

    m44 o = {};
    o.v[0][0] = sx * mtx.x.x;
    o.v[0][1] = sy * mtx.x.y;
    o.v[0][2] = sz * mtx.x.z;
    o.v[0][3] = mtx.x.z;

    o.v[1][0] = sx * mtx.y.x;
    o.v[1][1] = sy * mtx.y.y;
    o.v[1][2] = sz * mtx.y.z;
    o.v[1][3] = mtx.y.z;

    o.v[2][0] = sx * mtx.z.x;
    o.v[2][1] = sy * mtx.z.y;
    o.v[2][2] = sz * mtx.z.z;
    o.v[2][3] = mtx.z.z;

    o.v[3][0] = sx * mtx.w.x;
    o.v[3][1] = sy * mtx.w.y;
    o.v[3][2] = sz * mtx.w.z - near_v * d;
    o.v[3][3] = mtx.w.z + 1;
    return o;
}

// Screen to Clip coordinates
//
//      | top_left |  bottom_right
// -----+----------+---------------
// From |   0, 0   | width,  height
// To   |  -1, 1   |     1,      -1
//
static void m4_screen_to_clip(m4 *m, v2 size) {
    f32 sx = 2.0 / size.x;
    f32 sy = 2.0 / size.y;
    m4_scale(m, (v3){sx, -sy, 1});
    m4_trans(m, (v3){-1, 1, 0});
}
