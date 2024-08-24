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

// Rotate around the Z axis
// m = Rz(a) * m
static void m4_rot_z(m4 *m, f32 a) {
    f32 c = f_cos(a);
    f32 s = f_sin(a);

    m4s fwd = {
        .x = {c, s, 0, 0},
        .y = {-s, c, 0, 0},
        .z = {0, 0, 1, 0},
        .w = {0, 0, 0, 1},
    };

    m4s inv = {
        .x = {c, -s, 0, 0},
        .y = {s, c, 0, 0},
        .z = {0, 0, 1, 0},
        .w = {0, 0, 0, 1},
    };

    m4_mul(m, &(m4){fwd, inv});
}

// Rotate around the Y axis
// m = Ry(a) * m
static void m4_rot_y(m4 *m, f32 a) {
    f32 c = f_cos(a);
    f32 s = f_sin(a);
    m4s fwd = {
        .x = {c, 0, -s, 0},
        .y = {0, 1, 0, 0},
        .z = {s, 0, c, 0},
        .w = {0, 0, 0, 1},
    };

    m4s inv = {
        .x = {c, 0, s, 0},
        .y = {0, 1, 0, 0},
        .z = {-s, 0, c, 0},
        .w = {0, 0, 0, 1},
    };

    m4_mul(m, &(m4){fwd, inv});
}

// Rotate around the X axis
// m = Rx(a) * m
static void m4_rot_x(m4 *m, f32 a) {
    f32 c = f_cos(a);
    f32 s = f_sin(a);
    m4s fwd = {
        .x = {1, 0, 0, 0},
        .y = {0, c, s, 0},
        .z = {0, -s, c, 0},
        .w = {0, 0, 0, 1},
    };
    m4s inv = {
        .x = {1, 0, 0, 0},
        .y = {0, c, -s, 0},
        .z = {0, s, c, 0},
        .w = {0, 0, 0, 1},
    };
    m4_mul(m, &(m4){fwd, inv});
}

// Compute the perspective projection matrix
// x: Left   --> Right
// y: Bottom --> Top
// z: Far    --> Near
static void m4_perspective_to_clip(m4 *mtx, f32 fov, f32 aspect, f32 near_v, f32 far_v) {
    f32 tan_fov = f_tan(fov * DEG_TO_RAD / 2.0f);

    m4 m = {};

    f32 a = aspect * tan_fov;
    f32 b = tan_fov;
    f32 d = (near_v + far_v) / (far_v - near_v);
    f32 e = (2 * far_v * near_v) / (far_v - near_v);

    // Perspective Projection Matrix
    // Maps 3D -> 2D
    m.fwd.x.x = 1.0f / a;
    m.fwd.y.y = 1.0f / b;
    m.fwd.z.z = -d;
    m.fwd.w.z = -e;
    m.fwd.z.w = -1.0f;

    // Inverse Perspective Projection Matrix
    // Maps 3D <- 2D
    m.inv.x.x = a;
    m.inv.y.y = b;
    m.inv.w.z = -1.0f;
    m.inv.z.w = -1.0 / e;
    m.inv.w.w = d / e;

    // Check
    // m4s check = m4s_mul(&m.fwd, &m.inv);

    // Apply
    m4_mul(mtx, &m);
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
