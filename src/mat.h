// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// mat.h - Math for 4d matrices on homogeneous coordinates (including the matrix inverse)
#pragma once
#include "mat_single.h"
#include "vec.h"

// A 4x4 Opengl column major matrix
typedef struct {
    f32 v[4][4];
} m44;

// Compute the perspective projection matrix
//
// OpenGL clip coordinates:
//
// +y is up
// +x is right
// -z is forwards
//
// Clip space ranges from -1 to 1 for each of these axis
//
// See: https://www.youtube.com/watch?v=U0_ONQQ5ZNM
//
// OpenGL performs a 'z-divide' for us:
//
// | x |     
// | y |     | x / w |
// | z | <=> | y / w |
// | w |     | z / w |
//
// so <1, 2, 3, 1>  = <10, 20, 30, 10> = <2, 3, 6, 2> = <1, 2, 3>
//

//         .               O
//    .    |              /|\
// 0-------|--------------/ \
// <-- n -->
// <--------- z ---------->
//
// x = x / z * n
// y = y / z * n
//
// We want the following to divide by z
//   (nx, ny, z*z, z) --> (nx/z, ny/z, z)
//
// But that is not possible because squaring is not a linear operation
// However we can solve the equation for near and far.
//
// | n 0 0 0 |
// | 0 n 0 0 |
// | 0 0 a b |
// | 0 0 1 0 |
// 
// solving:
//   a*z + b == z*z
//   for z=n, z=f
//
// gives:
//   a*n + b = n*n
//   a*f + b = f*f
//
// gives:
//   a = f+n
//   b = -f*n
//
// 

static m44 m4_perspective_to_clip(m4s mtx, f32 vertical_fov, f32 aspect_w_over_h, f32 near_v, f32 far_v) {
    f32 tan_vertical_fov = f_tan(vertical_fov * DEG_TO_RAD * 0.5f);

    // Perspective Projection Matrix
    // Maps 3D -> 2D
    // also rotate 180 degree (because 0,0 is top left)
    f32 sx = 1.0f / (aspect_w_over_h * tan_vertical_fov);
    f32 sy = 1.0f / tan_vertical_fov;
    f32 sz = far_v / (far_v - near_v);
    f32 tz = -far_v * near_v / (far_v - near_v);

    // | sx  0  0  0 |
    // | 0  sy  0  0 |
    // | 0   0 sz tz |
    // | 0   0  1  0 |


    // | sx  0  0  0 |   | xx yx zx wx |   | sx*xx   sx*yx   sx*zx   sx*wx      |
    // | 0  sy  0  0 |   | xy yy zy wy |   | sy*xy   sy*yy   sy*zy   sy*wy      |
    // | 0   0 sz tz | x | xz yz zz wz | = | sz*xz   sz*yz   sz*zz   sz*wz + tz |
    // | 0   0  1  0 |   |  0  0  0  1 |   |   xz      yz      zz      wz       |
    
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
    o.v[3][2] = sz * mtx.w.z + tz;
    o.v[3][3] = mtx.w.z;
    return o;
}

// Screen to Clip coordinates
//
//      | top_left |  bottom_right
// -----+----------+---------------
// From |   0, 0   | width,  height
// To   |  -1, 1   |     1,      -1
//
static m44 m4_screen_to_clip(m4s mtx, v2 size) {
    f32 sx =  2.0 / size.x;
    f32 sy = -2.0 / size.y;
    f32 sz = 1.0;

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

    o.v[3][0] = sx * mtx.w.x - 1;
    o.v[3][1] = sy * mtx.w.y + 1;
    o.v[3][2] = sz * mtx.w.z;
    o.v[3][3] = 0;
    return o;
}
