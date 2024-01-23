// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// mat.h - Math for 4d matrices on homogeneous coordinates
#pragma once
#include "vec.h"

// Single 4x4 matrix
// column major
// | X.x   Y.x   Z.x   W.x |
// |                       |
// | X.y   Y.y   Z.y   W.y |
// |                       |
// | X.z   Y.z   Z.z   W.z |
// |                       |
// | X.w   Y.w   Z.w   W.w |
struct m4_single {
    v4 x;
    v4 y;
    v4 z;
    v4 w;
};

// Double 4x4 matrix containing both the forward and inverse matrix
// A matrix inverse is complicated, so we use more memory to track the inverse directly
// M.fwd * M.inv == M.inv * M.fwd == m4_id()
struct m4 {
    m4_single fwd;
    m4_single inv;
};

// Identity matrix, does nothing
static m4 m4_id(void) {
    m4_single id = {
        .x = {1, 0, 0, 0},
        .y = {0, 1, 0, 0},
        .z = {0, 0, 1, 0},
        .w = {0, 0, 0, 1},
    };
    return (m4){id, id};
}

// Invert the matrix.
// This is a cheap operation in this case because we already computed the inverse
static void m4_inv(m4 *m) {
    *m = (m4){m->inv, m->fwd};
}

// Multiply a 4 component vector with this matrix
// y = M * x
static v4 m4_mul_v4(m4_single *m, v4 v) {
    return v.x * m->x + v.y * m->y + v.z * m->z + v.w * m->w;
}

// Transform a position by the transformation matrix
static v3 m4_mul_pos(m4_single *m, v3 v) {
    return (v.x * m->x + v.y * m->y + v.z * m->z + m->w).xyz;
}

// Transform a direction by the transformation matrix
static v3 m4_mul_dir(m4_single *m, v3 v) {
    return (v.x * m->x + v.y * m->y + v.z * m->z).xyz;
}

// C = A * B
static m4_single m4_mul_single(m4_single *a, m4_single *b) {
    return (m4_single){
        .x = m4_mul_v4(a, b->x),
        .y = m4_mul_v4(a, b->y),
        .z = m4_mul_v4(a, b->z),
        .w = m4_mul_v4(a, b->w),
    };
}

// left multiply by b
// m = b * m
static void m4_mul(m4 *m, m4 *b) {
    // m.fwd = b.fwd * m.fwd
    // m.inv = m.inv * b.inv
    m->fwd = m4_mul_single(&b->fwd, &m->fwd);
    m->inv = m4_mul_single(&m->inv, &b->inv);
}

// Scale on the x, y, and z axis
// m = S * m
static void m4_scale(m4 *m, v3 s) {
    m4_mul(m, &(m4) {
        .fwd = {
            .x = { s.x,   0,   0, 0 },
            .y = {   0, s.y,   0, 0 },
            .z = {   0,   0, s.z, 0 },
            .w = {   0,   0,   0, 1 },
        },
        .inv = {
            .x = { 1/s.x,     0,     0, 0 },
            .y = {     0, 1/s.y,     0, 0 },
            .z = {     0,     0, 1/s.z, 0 },
            .w = {     0,     0,     0, 1 },
        },
    });
}

// Translate the matrix
// m = T * m
static void m4_trans(m4 *m, v3 p) {
    m4_mul(m, &(m4) {
        .fwd = {
            .x = {   1,   0,   0, 0 },
            .y = {   0,   1,   0, 0 },
            .z = {   0,   0,   1, 0 },
            .w = { p.x, p.y, p.z, 1 },
        },
        .inv = {
            .x = {    1,    0,    0, 0 },
            .y = {    0,    1,    0, 0 },
            .z = {    0,    0,    1, 0 },
            .w = { -p.x, -p.y, -p.z, 1 },
        },
    });
}


// Rotate around the Z axis
// m = Rz(a) * m
static void m4_rot_z(m4 *m, f32 a) {
    f32 c = f_cos(a);
    f32 s = f_sin(a);
    m4_mul(m, &(m4) {
        .fwd = {
            .x = { c, s, 0, 0 },
            .y = {-s, c, 0, 0 },
            .z = { 0, 0, 1, 0 },
            .w = { 0, 0, 0, 1 },
        },
        .inv = {
            .x = { c,-s, 0, 0 },
            .y = { s, c, 0, 0 },
            .z = { 0, 0, 1, 0 },
            .w = { 0, 0, 0, 1 },
        }
    });
}

// Rotate around the Y axis
// m = Ry(a) * m
static void m4_rot_y(m4 *m, f32 a) {
    f32 c = f_cos(a);
    f32 s = f_sin(a);
    m4_mul(m, &(m4) {
        .fwd = {
            .x = { c, 0,-s, 0 },
            .y = { 0, 1, 0, 0 },
            .z = { s, 0, c, 0 },
            .w = { 0, 0, 0, 1 },
        },
        .inv = {
            .x = { c, 0, s, 0 },
            .y = { 0, 1, 0, 0 },
            .z = {-s, 0, c, 0 },
            .w = { 0, 0, 0, 1 },
        }
    });
}

// Rotate around the X axis
// m = Rx(a) * m
static void m4_rot_x(m4 *m, f32 a) {
    f32 c = f_cos(a);
    f32 s = f_sin(a);
    m4_mul(m, &(m4) {
        .fwd = {
            .x = {  1, 0, 0, 0 },
            .y = {  0, c, s, 0 },
            .z = {  0,-s, c, 0 },
            .w = {  0, 0, 0, 1 },
        },
        .inv = {
            .x = {  1, 0, 0, 0 },
            .y = {  0, c,-s, 0 },
            .z = {  0, s, c, 0 },
            .w = {  0, 0, 0, 1 },
        }
    });
}

// Compute the perspective projection matrix
// TODO: define axis and orientation
static m4 m4_perspective_to_clip(m4 *mtx, f32 fov, f32 aspect_ratio, f32 near_v, f32 far_v) {
    f32 focal_length = 1 / f_tan(fov * DEG_TO_RAD / 2);

    f32 ax = 1 / aspect_ratio;
    f32 ay = 1;

    f32 d = (near_v + far_v) / (near_v - far_v);
    f32 e = (2 * far_v * near_v) / (near_v - far_v);

    m4 m = {};
    m.inv.x.x = ax * focal_length;
    m.inv.y.y = -ay * focal_length;
    m.inv.z.z = d;
    m.inv.w.z = e;
    m.inv.z.w = -1;

    m.fwd.x.x = 1 / (ax * focal_length);
    m.fwd.y.y = 1 / (ay * focal_length);
    m.fwd.w.z = -1;
    m.fwd.z.w = 1 / e;
    m.fwd.w.w = d / e;
    m4_inv(&m);
    m4_mul(mtx, &m);
    return m;
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
    m4_trans(m, (v3){-1,  1, 0});
}
