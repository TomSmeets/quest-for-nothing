// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// mat_single.h - Math for 4d matrices on homogeneous coordinates
#pragma once
#include "vec.h"

// Single 4x4 matrix
// column major, just like OpenGL
// This allows us to extract the columns directly
// | X.x   Y.x   Z.x   W.x |
// |                       |
// | X.y   Y.y   Z.y   W.y |
// |                       |
// | X.z   Y.z   Z.z   W.z |
// |                       |
// | X.w   Y.w   Z.w   W.w |
typedef struct {
    // Rotation
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
static m4s m4s_scale(m4s m, v3 s) {
    m.x*=s;
    m.y*=s;
    m.z*=s;
    m.w*=s;
    return m;
}

// T * M
static m4s m4s_translate(m4s m, v3 t) {
    m.w += t;
    return m;
}

// Rotate around the Z axis
// R * M
static m4s m4_rot_z(m4s m, f32 a) {
    f32 c = f_cos(a);
    f32 s = f_sin(a);
    return m4s_mul((m4s) {
        .x = {c, s, 0},
        .y = {-s, c, 0},
        .z = {0, 0, 1},
    }, m);
}

// Rotate around the Y axis
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

// Rotate around the X axis
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

// Invert translation+rotation matrix (No scaling!)
// https://www.youtube.com/watch?v=7CxKAtWqHC8
static m4s m4s_invert_tr(m4s m) {
    // TR matrix inverse
    // inv(T * R) = inv(R) * inv(T)

    // 3x3 rotation matrix inverse is it's transpose
    m4s inv_r = {
        .x = {m.x.x, m.y.x, m.z.x},
        .y = {m.x.y, m.y.y, m.z.y},
        .z = {m.x.z, m.y.z, m.z.z},
    };

    // inv(T)
    v3 inv_t = -m.w;

    // inv(R) * inv(T)
    inv_r.w = m4s_mul_pos(inv_r, inv_t);
    return inv_r;
}

// Invert translation+rotation matrix (No scaling!)
// https://www.youtube.com/watch?v=7CxKAtWqHC8
static m4s m4s_invert(m4s m) {
    // inv(T * R * S) = inv(S) * inv(R) * inv(T)
    v3 t = m.w;
    v3 inv_t = -t;

    v3 s = { v3_length(m.x), v3_length(m.y), v3_length(m.z) };
    v3 inv_s = { 1.0f / s.x, 1.0f / s.y, 1.0f / s.z };

    v3 rx = m.x * inv_s.x;
    v3 ry = m.y * inv_s.y;
    v3 rz = m.z * inv_s.z;

    // inv(R)
    m4s inv = {
        .x = {rx.x, ry.x, rz.x},
        .y = {rx.y, ry.y, rz.y},
        .z = {rx.z, ry.z, rz.z},
    };

    // inv(R) * inv(T)
    inv.w = m4s_mul_pos(inv, inv_t);

    // inv(S) * inv(R) * inv(T)
    inv = m4s_scale(inv, inv_s);

    return inv;
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

    mtx.w += mtx.x*wiggle;
    return mtx;
}
