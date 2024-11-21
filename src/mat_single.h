// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// mat_single.h - Math for 4d matrices on homogeneous coordinates
#pragma once
#include "vec.h"

// 4x4 matrix in column-major order (OpenGL style)
// Columns can be directly extracted:
// | X.x   Y.x   Z.x   W.x |
// |                       |
// | X.y   Y.y   Z.y   W.y |
// |                       |
// | X.z   Y.z   Z.z   W.z |
// |                       |
// |   0     0     0     1 |
typedef struct {
    // Rotation + Scale
    v3 x;
    v3 y;
    v3 z;

    // Translation
    v3 w;
} m4s;

// Transform a direction by the transformation matrix
// R * x
static v3 m4s_mul_dir(m4s m, v3 v) {
    return m.x*v.x + m.y*v.y + m.z*v.z;
}

// Transform a position by the transformation matrix
// R * x + T
static v3 m4s_mul_pos(m4s m, v3 v) {
    return m4s_mul_dir(m, v) + m.w;
}

// C = A * B
static m4s m4s_mul(m4s a, m4s b) {
    return (m4s){
        .x = a.x*b.x.x + a.y*b.x.y + a.z*b.x.z,
        .y = a.x*b.y.x + a.y*b.y.y + a.z*b.y.z,
        .z = a.x*b.z.x + a.y*b.z.y + a.z*b.z.z,
        .w = a.x*b.w.x + a.y*b.w.y + a.z*b.w.z + a.w,
    };
}

// Matrix identity
// I
static m4s m4s_id(void) {
    return (m4s){
        .x = {1, 0, 0},
        .y = {0, 1, 0},
        .z = {0, 0, 1},
        .w = {0, 0, 0},
    };
}

// S * M
//
// | S 0 |   | A B |   | SA SB |
// | 0 1 | x | 0 1 | = |  0  1 |
static m4s m4s_scale(m4s m, v3 s) {
    m.x*=s;
    m.y*=s;
    m.z*=s;
    m.w*=s;
    return m;
}

// T * M
// 
// | I T |   | A B |   | A B+T |
// | 0 1 | x | 0 1 | = | 0  1  |
static m4s m4s_translate(m4s m, v3 t) {
    m.w += t;
    return m;
}

// Right handed rotation around the X-axis (Y -> Z)
// R * M
static m4s m4_rot_x(m4s m, f32 a) {
    f32 c = f_cos(a);
    f32 s = f_sin(a);
    return m4s_mul((m4s) {
        .x = {1, 0, 0},
        .y = {0, c, s},
        .z = {0, -s, c},
        .w = {0, 0, 0},
    }, m);
}

// Right handed rotation around the Y-axis (Z -> X)
// R * M
static m4s m4_rot_y(m4s m, f32 a) {
    f32 c = f_cos(a);
    f32 s = f_sin(a);
    return m4s_mul((m4s) {
        .x = { c, 0, -s},
        .y = { 0, 1, 0 },
        .z = { s, 0, c },
    }, m);
}

// Right handed rotation around the Z-axis (X -> Y)
// R * M
//
// | R 0 |   | A B |   | RA RB |
// | 0 1 | x | 0 1 | = | 0   1 |
static m4s m4_rot_z(m4s m, f32 a) {
    f32 c = f_cos(a);
    f32 s = f_sin(a);
    return m4s_mul((m4s) {
        .x = {c, s, 0},
        .y = {-s, c, 0},
        .z = {0, 0, 1},
    }, m);
}

// Invert translation+rotation matrix (No scaling!)
// https://www.youtube.com/watch?v=7CxKAtWqHC8
//
//
// A TR matrix looks like this:
// | I T |   | R 0 |   | R T |
// | 0 1 | x | 0 1 | = | 0 1 |
//
// TR matrix inverse
//
// inv(TR) = inv(R) * inv(T)
// inv(R) = transpose(R)
// inv(T) = -T
// transpose(R) * (-T)
//
// | Rt 0 |   | 1 -T |   | Rt  Rt*(-T) |
// | 0  1 | x | 0  1 | = | 0      1    |
static m4s m4s_invert_tr(m4s m) {
    // 3x3 rotation matrix inverse is it's transpose
    m4s inv_r = {
        .x = {m.x.x, m.y.x, m.z.x},
        .y = {m.x.y, m.y.y, m.z.y},
        .z = {m.x.z, m.y.z, m.z.z},
    };

    // Translation inverse is its negative
    v3 inv_t = -m.w;

    // inv(R) * inv(T)
    inv_r.w = m4s_mul_pos(inv_r, inv_t);
    return inv_r;
}

// Render a flat upright sprite facing the camera
static m4s m4s_billboard(v3 pos, v3 target, float wiggle) {
    // Relative direction to the camera in xz
    v2 fwd = v2_normalize(pos.xz - target.xz);

    m4s mtx = {
        .x = { fwd.y, 0, -fwd.x },
        .y = {0, 1, 0},
        .z = { fwd.x, 0, fwd.y},
        .w = pos + (v3) { 0, 0.5, 0 }
    };

    // Wiggle a little on the x axis
    mtx.w += mtx.x * wiggle;
    return mtx;
}
