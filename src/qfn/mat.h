// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// mat.h - Math for 4d matrices on homogeneous coordinates
#pragma once
#include "lib/vec.h"

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
} m4;

// Transform a direction by the transformation matrix
// R * x
static v3 m4_mul_dir(m4 m, v3 v) {
    return m.x * v.x + m.y * v.y + m.z * v.z;
}

// Transform a position by the transformation matrix
// R * x + T
static v3 m4_mul_pos(m4 m, v3 v) {
    return m4_mul_dir(m, v) + m.w;
}

// C = A * B
static m4 m4_mul(m4 a, m4 b) {
    return (m4){
        .x = a.x * b.x.x + a.y * b.x.y + a.z * b.x.z,
        .y = a.x * b.y.x + a.y * b.y.y + a.z * b.y.z,
        .z = a.x * b.z.x + a.y * b.z.y + a.z * b.z.z,
        .w = a.x * b.w.x + a.y * b.w.y + a.z * b.w.z + a.w,
    };
}

// M = A * M
static void m4_apply(m4 *m, m4 a) {
    *m = m4_mul(a, *m);
}

// Matrix identity
// I
static m4 m4_id(void) {
    return (m4){
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
static void m4_scale(m4 *m, v3 s) {
    m->x *= s;
    m->y *= s;
    m->z *= s;
    m->w *= s;
}

// T * M
//
// | I T |   | A B |   | A B+T |
// | 0 1 | x | 0 1 | = | 0  1  |
static void m4_translate(m4 *m, v3 t) {
    m->w += t;
}

static void m4_translate_x(m4 *m, f32 x) {
    m->w.x += x;
}

static void m4_translate_y(m4 *m, f32 y) {
    m->w.y += y;
}

static void m4_translate_z(m4 *m, f32 z) {
    m->w.z += z;
}

// Right handed rotation around the X-axis (Y -> Z)
// R * M
static void m4_rotate_x(m4 *m, f32 a) {
    f32 c = f_cos(a);
    f32 s = f_sin(a);
    m4_apply(
        m, (m4){
               .x = {1, 0, 0},
               .y = {0, c, s},
               .z = {0, -s, c},
           }
    );
}

// Right handed rotation around the Y-axis (Z -> X)
// R * M
static void m4_rotate_y(m4 *m, f32 a) {
    f32 c = f_cos(a);
    f32 s = f_sin(a);
    m4_apply(
        m, (m4){
               .x = {c, 0, -s},
               .y = {0, 1, 0},
               .z = {s, 0, c},
           }
    );
}

// Right handed rotation around the Z-axis (X -> Y)
// R * M
//
// | R 0 |   | A B |   | RA RB |
// | 0 1 | x | 0 1 | = | 0   1 |
static void m4_rotate_z(m4 *m, f32 a) {
    f32 c = f_cos(a);
    f32 s = f_sin(a);
    m4_apply(
        m, (m4){
               .x = {c, s, 0},
               .y = {-s, c, 0},
               .z = {0, 0, 1},
           }
    );
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
static m4 m4_invert_tr(m4 m) {
    f32 l1 = v3_length_sq(m.x);
    f32 l2 = v3_length_sq(m.y);
    f32 l3 = v3_length_sq(m.z);

    if (0) {
        assert(l1 <= 1.0f + 0.01 && l1 >= 1.0f - 0.01, "Matrix has scaling in x");
        assert(l2 <= 1.0f + 0.01 && l2 >= 1.0f - 0.01, "Matrix has scaling in y");
        assert(l3 <= 1.0f + 0.01 && l3 >= 1.0f - 0.01, "Matrix has scaling in z");
    } else {
        m.x *= 1.0 / l1;
        m.y *= 1.0 / l2;
        m.z *= 1.0 / l3;
    }

    // 3x3 rotation matrix inverse is it's transpose
    m4 inv_r = {
        .x = {m.x.x, m.y.x, m.z.x},
        .y = {m.x.y, m.y.y, m.z.y},
        .z = {m.x.z, m.y.z, m.z.z},
    };

    // Translation inverse is its negative
    v3 inv_t = -m.w;

    // inv(R) * inv(T)
    inv_r.w = m4_mul_pos(inv_r, inv_t);
    return inv_r;
}

// Rotate matrix by Yaw, Pitch and Roll
static void m4_rotate(m4 *mtx, v3 rot) {
    m4_rotate_z(mtx, rot.z);
    m4_rotate_x(mtx, rot.x);
    m4_rotate_y(mtx, rot.y);
}

// Render a flat upright sprite facing the camera
static m4 m4_billboard(v3 pos, v3 forward, float wiggle, float death) {
    // Relative direction to the camera in xz
    v2 fwd = forward.xz;

    // right = x
    // up    = y
    // fwd   = -z

    v3 x = {-fwd.y, 0, fwd.x};
    v3 y = {0, 1, 0};
    v3 z = {fwd.x, 0, fwd.y};
    v3 w = pos;
    m4 body = {x, y, z, w};

    death *= R1;
    m4 mtx = m4_id();
    // m4_translate(&mtx, (v3){0, -.495f * (1.0f - f_cos(death)), 0});
    // m4_translate(&mtx, (v3){0, 0.5, 0});
    // m4_scale(&mtx, (v3){size.x, size.y, size.x});
    m4_rotate_x(&mtx, -death);
    m4_rotate_z(&mtx, wiggle * PI);
    // m4_translate(&mtx, (v3){0, 0.02, 0});
    m4_apply(&mtx, body);
    return mtx;
}

// A 4x4 Opengl column major matrix
typedef struct {
    f32 v[4][4];
} m44;

// Compute the perspective projection matrix
//
// OpenGL clip coordinates:
//
// +y is up
// +x is left
// +z is forwards
//
// Clip space ranges from -1 to 1 for each of these axis
//
// Good videos by OGLDEV
// - Perspective Projection - Part1: https://www.youtube.com/watch?v=LhQ85bPCAJ8
// - Perspective Projection - Part2: https://www.youtube.com/watch?v=md3jFANT3UM
//
// Another explanation by Brendan Galea, but Z calculation is incorrect for OpenGL, clip space is form -1 to 1, not near to far.
// - The Math behind (most) 3D games - Perspective Projection: https://www.youtube.com/watch?v=U0_ONQQ5ZNM
static m44 m4_perspective_to_clip(m4 mtx, f32 vertical_fov, f32 aspect_x, f32 aspect_y, f32 near_v, f32 far_v) {
    f32 tan_vertical_fov = f_tan(vertical_fov * DEG_TO_RAD * 0.5f);

    // Right handed Perspective Projection Matrix
    // | sx   0   0   0 |
    // |  0  sy   0   0 |
    // |  0   0  sz  tz |
    // |  0   0  sw   0 |
    f32 sx = -1.0f / (tan_vertical_fov * aspect_x);
    f32 sy = 1.0f / (tan_vertical_fov * aspect_y);
    f32 sz = (far_v + near_v) / (far_v - near_v);
    f32 tz = (2 * near_v * far_v) / (near_v - far_v);
    f32 sw = 1;

    // Solving for near and far planes:
    // (sz*z + tz) / z = z1
    // sz + tz/z       = z1
    //
    // sz + tz/n = -1
    // sz + tz/f = +1
    //
    // | 1  1/n | -1 |
    // | 1  1/f | +1 |
    //
    // | 0  1/n-1/f | -2 |
    // | 1      1/f | +1 |
    //
    // | 0      n-f | 2nf |
    // | 1      1/f | +1 |
    //
    // | 0      1   | 2nf/(n-f) |
    // | 1      1/f | +1 |
    //
    // | 0      1   | 2nf/(n-f) |
    // | 1      0   | 1 - 2n/(n-f) |
    //
    // | 0      1   | 2nf/(n-f) |
    // | 1      0   | (f + n)/(f-n) |

    // | sx  0  0  0 |   | xx yx zx wx |   | sx*xx   sx*yx   sx*zx   sx*wx      |
    // | 0  sy  0  0 |   | xy yy zy wy |   | sy*xy   sy*yy   sy*zy   sy*wy      |
    // | 0   0 sz tz | x | xz yz zz wz | = | sz*xz   sz*yz   sz*zz   sz*wz + tz |
    // | 0   0 sw  0 |   |  0  0  0  1 |   | sw*xz   sw*yz   sw*zz   sw*wz      |

    // mul
    // P * M

    m44 o = {};
    o.v[0][0] = sx * mtx.x.x;
    o.v[0][1] = sy * mtx.x.y;
    o.v[0][2] = sz * mtx.x.z;
    o.v[0][3] = sw * mtx.x.z;

    o.v[1][0] = sx * mtx.y.x;
    o.v[1][1] = sy * mtx.y.y;
    o.v[1][2] = sz * mtx.y.z;
    o.v[1][3] = sw * mtx.y.z;

    o.v[2][0] = sx * mtx.z.x;
    o.v[2][1] = sy * mtx.z.y;
    o.v[2][2] = sz * mtx.z.z;
    o.v[2][3] = sw * mtx.z.z;

    o.v[3][0] = sx * mtx.w.x;
    o.v[3][1] = sy * mtx.w.y;
    o.v[3][2] = sz * mtx.w.z + tz;
    o.v[3][3] = sw * mtx.w.z;
    return o;
}

// Screen to Clip coordinates (scaled only, 0,0 is center)
// 0,0,0 -> 0,0,0
static m44 m4_screen_to_clip(m4 mtx, v2i size) {
    f32 sx = 2.0f / (f32)size.x;
    f32 sy = 2.0f / (f32)size.y;
    f32 sz = 1.0f;

    // | sx  0  0  0 |
    // | 0  sy  0  0 |
    // | 0   0  1  0 |
    // | 0   0  0  1 |

    // | sx  0  0  0 |   | xx yx zx wx |   | sx*xx   sx*yx   sx*zx   sx*wx |
    // | 0  sy  0  0 |   | xy yy zy wy |   | sy*xy   sy*yy   sy*zy   sy*wy |
    // | 0   0 sz  0 | x | xz yz zz wz | = | sz*xz   sz*yz   sz*zz   sz*wz |
    // | 0   0  0  1 |   |  0  0  0  1 |   |   0       0       0      1    |
    m44 o = {};
    o.v[0][0] = sx * mtx.x.x;
    o.v[0][1] = sy * mtx.x.y;
    o.v[0][2] = sz * mtx.x.z;
    o.v[0][3] = 0;

    o.v[1][0] = sx * mtx.y.x;
    o.v[1][1] = sy * mtx.y.y;
    o.v[1][2] = sz * mtx.y.z;
    o.v[1][3] = 0;

    o.v[2][0] = sx * mtx.z.x;
    o.v[2][1] = sy * mtx.z.y;
    o.v[2][2] = sz * mtx.z.z;
    o.v[2][3] = 0;

    o.v[3][0] = sx * mtx.w.x;
    o.v[3][1] = sy * mtx.w.y;
    o.v[3][2] = sz * mtx.w.z;
    o.v[3][3] = 1;
    return o;
}
