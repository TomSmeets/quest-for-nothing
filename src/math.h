// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// math.h - float, integer and vector math expressions
#pragma once
#include "inc.h"

#define PI  3.1415926535897931
#define PI2 6.2831853071795862

// 360, 180, and 90 turn
#define R4 6.2831853071795862
#define R2 3.1415926535897931
#define R1 1.5707963267948966

#define DEG_TO_RAD (R4 / 360.0)

// ==== Simple operations ====

// Absolute value of x
static f32 f_abs(f32 x) { return (x < 0) ? -x : x; }

// Round towards -Inf
static i32 f_floor(f32 x) { return (i32)x - (x < (i32)x); }

// Round towards 0
static i32 f_trunc(f32 x) { return (i32)x; }

// return the fractional part
// always positive and in the range [0,1)
static f32 f_fract(f32 x) { return x - f_floor(x); }

// limit v to [min, max] but wrapping around when exceeding this range
static f32 f_wrap(f32 v, f32 min, f32 max) {
    f32 r = max - min;
    return f_fract((v - min) / r) * r + min;
}

static f32 f_min(f32 a, f32 b) { return a <= b ? a : b; }
static i32 i_min(i32 a, i32 b) { return a <= b ? a : b; }
static u32 u_min(u32 a, u32 b) { return a <= b ? a : b; }

static f32 f_max(f32 a, f32 b) { return a >= b ? a : b; }
static i32 i_max(i32 a, i32 b) { return a >= b ? a : b; }
static u32 u_max(u32 a, u32 b) { return a >= b ? a : b; }

// clamp 'x' between l and h, inclusive
static i32 i_clamp(i32 x, i32 l, i32 h) {
    if (x < l) x = l;
    if (x > h) x = h;
    return x;
}

// clamp 'x' between l and h, inclusive
static f32 f_clamp(f32 x, f32 l, f32 h) {
    if (x < l) x = l;
    if (x > h) x = h;
    return x;
}

// ==== Trigonometric functions ====

// Very good and simple sine approximation
static f32 f_sin2pi(f32 x) {
    x = x - (u32) (x + .5f);
    f32 y = 8 * x - 16 * x * f_abs(x);
    return 0.225f * (y * f_abs(y) - y) + y;
}

static f32 f_cos2pi(f32 x) {
    return f_sin2pi(x + .25);
}

static f32 f_sin(f32 x) { return f_sin2pi(x * 1.0 / R4); }
static f32 f_cos(f32 x) { return f_cos2pi(x * 1.0 / R4); }

// fast tangent approximation
static f32 f_tan(f32 x) { return f_sin(x) / f_cos(x); }

// fast inverse tangent approximation
static f32 f_atan(f32 x) {
    return 0.5 * R1 * x - x * (f_abs(x) - 1) * (0.2447 + 0.0663 * f_abs(x));
}

// fast 2 component inverse tangent approximation
// calculates the angle of (x, y) to the x axis
static f32 f_atan2(f32 y, f32 x) {
    // edge cases
    if (x == 0 && y == 0) return   0;
    if (x == 0 && y >  0) return  R1;
    if (x == 0 && y <  0) return -R1;
    if (y == 0 && x >  0) return   0;
    if (y == 0 && x <  0) return  R2;

    // Top Right
    // The unit circle divided into 8 sections
    if (x > 0 && y > 0 &&  x >=  y) return   0 + f_atan(y / x);
    if (x > 0 && y > 0 &&  x <   y) return  R1 - f_atan(x / y);

    if (x < 0 && y > 0 && -x <=  y) return  R1 + f_atan(-x / y);
    if (x < 0 && y > 0 && -x >   y) return  R2 - f_atan(-y / x);

    if (x < 0 && y < 0 && -x >= -y) return -R2 + f_atan(y / x);
    if (x < 0 && y < 0 && -x <  -y) return -R1 - f_atan(x / y);

    if (x > 0 && y < 0 &&  x <= -y) return -R1 + f_atan(-x / y);
    if (x > 0 && y < 0 &&  x >  -y) return   0 - f_atan(-y / x);

    // should not happen
    assert(0);
    return 0;
}

static f32 bits_to_float(u32 u) {
    union {
        f32 f;
        u32 u;
    } conv;
    conv.u = u;
    return conv.f;
}

// https://gist.github.com/petrsm/079de9396d63e00d5994a7cc936ae9c7
// seems pretty accurate
static f32 f_pow2(f32 x) {
    // 2^x = 2^i * 2^f
    i32 pi = f_floor(x); // integer part
    f32 pf = x - pi;     // fractional part, 0-1
    assert((f32)pi <= x);
    assert(pi >= -127 && pi <= 128);
    assert(pf >= 0 && pf <= 1);
    // integer part is simple, because we just write in the exp part of the float
    f32 pow2i = bits_to_float((1 << 23) * (pi + 127));
    f32 pow2f = 1.3697664475809267e-2f;
    pow2f = pow2f * pf + 5.1690358205939469e-2f;
    pow2f = pow2f * pf + 2.4163844572498163e-1f;
    pow2f = pow2f * pf + 6.9296612266139567e-1f;
    pow2f = pow2f * pf + 1.000003704465937f;
    return pow2i * pow2f;
}

static f32 f_inv_sqrt(f32 v) {
    union {
        f32 f;
        u32 i;
    } conv = {.f = v};
    conv.i = 0x5f3759df - (conv.i >> 1);
    conv.f *= 1.5f - (v * 0.5f * conv.f * conv.f);
    conv.f *= 1.5f - (v * 0.5f * conv.f * conv.f);
    return conv.f;
}

static f32 f_sqrt(f32 v) {
    return f_inv_sqrt(v) * v;
}

static f32 f_acos(f32 x) {
    return f_atan2(f_sqrt((1.0 + x) * (1.0 - x)), x);
}

static f32 f_asin(f32 x) {
    return f_atan2(x, f_sqrt((1.0 + x) * (1.0 - x)));
}

static f32 f_remap(f32 x, f32 xl, f32 xh, f32 yl, f32 yh) {
    f32 y = (x - xl) / (xh - xl);
    return y*(yh - yl) + yl;
}
