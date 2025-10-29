// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// math.h - float, integer and vector math expressions
#pragma once
#include "lib/types.h"

#define INF (1.0f / 0.0f)

#define PI 3.1415926535897931
#define PI2 6.2831853071795862

// 360, 180, and 90 turn
#define R4 6.2831853071795862
#define R2 3.1415926535897931
#define R1 1.5707963267948966

#define DEG_TO_RAD (PI2 / 360.0)

// ==== Simple operations ====

// Blend between two values
// a=0 => x, a=1 => y
#define BLEND(x, y, a) ((x) + ((y) - (x)) * (a))

// clang-format off

// Absolute value of x
static f32 f_abs(f32 x) { return (x < 0) ? -x : x; }
static u64 i_abs(i64 x) { return (x < 0) ? -x : x; }

// Round towards -Inf
static i32 f_floor(f32 x) { return (i32)x - (x < (i32)x); }
static i32 f_round(f32 x) { return f_floor(x+0.5); }

static f32 f_fract(f32 x) { return x - f_floor(x); }

// Round towards 0
static i32 f_trunc(f32 x) { return (i32)x; }

static f32 f_min(f32 a, f32 b) { return a <= b ? a : b; }
static i32 i_min(i32 a, i32 b) { return a <= b ? a : b; }
static u32 u_min(u32 a, u32 b) { return a <= b ? a : b; }

static f32 f_max(f32 a, f32 b) { return a >= b ? a : b; }
static i32 i_max(i32 a, i32 b) { return a >= b ? a : b; }
static u32 u_max(u32 a, u32 b) { return a >= b ? a : b; }

// clang-format on

// Exponential Step function from 0 to 1
static f32 f_step(f32 x) {
    if (x <= 0) return 0;
    if (x >= 1) return 1;
    x = 1 - x;
    x = x * x * x;
    x = 1 - x;
    return x;
}

static f32 bits_to_float(u32 u) {
    union {
        f32 f;
        u32 u;
    } conv;
    conv.u = u;
    return conv.f;
}

#if 1
static f32 f_exp(f32 x) {
    // Exp(x) = (1 + X/N)^N
    f32 y = 1.0f + x / 1024.0f;
    if (y < 0) return 0;
    for (u32 i = 0; i < 10; ++i) y *= y;
    return y;
}
#else
// Exponential approximation
static f32 f_exp(f32 x) {
    f32 a = (1 << 23) / 0.69314718f;
    f32 b = (1 << 23) * (127 - 0.043677448f);
    f32 y = a * x + b;

    // Remove these lines if bounds checking is not needed
    f32 c = (1 << 23);
    f32 d = (1 << 23) * 255;
    if (y < c) y = 0;
    if (y > d) y = d;
    return bits_to_float((u32)y);
}
#endif

static f32 f_step_duration(f32 t, f32 duration) {
    if (duration == 0) {
        return t >= 0 ? 1 : 0;
    }

    return f_step(t / duration);
}

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

// clamp 'x' between l and h, inclusive
static f32 f_wrap(f32 x, f32 l, f32 h) {
    assert(l < h, "Lower bound should be lower than higher bound");
    while (x < l) x += h - l;
    while (x > h) x += l - h;
    return x;
}

// ==== Trigonometric functions ====

// Very good and simple sine approximation
static f32 f_sin2pi(f32 x) {
    x = x - f_floor(x + .5f);
    f32 y = 8 * x - 16 * x * f_abs(x);
    return 0.225f * (y * f_abs(y) - y) + y;
}

static f32 f_cos2pi(f32 x) {
    return f_sin2pi(x + .25f);
}

static f32 f_sin(f32 x) {
    return f_sin2pi(x * 1.0f / PI2);
}
static f32 f_cos(f32 x) {
    return f_cos2pi(x * 1.0f / PI2);
}

// fast tangent approximation
static f32 f_tan(f32 x) {
    return f_sin(x) / f_cos(x);
}

// fast inverse tangent approximation
static f32 f_atan(f32 x) {
    return 0.5f * R1 * x - x * (f_abs(x) - 1) * (0.2447f + 0.0663f * f_abs(x));
}

// fast 2 component inverse tangent approximation
// calculates the angle of (x, y) to the x axis
static f32 f_atan2(f32 y, f32 x) {
    // edge cases
    if (x == 0 && y == 0) return 0;
    if (x == 0 && y > 0) return R1;
    if (x == 0 && y < 0) return -R1;
    if (y == 0 && x > 0) return 0;
    if (y == 0 && x < 0) return R2;

    // Top Right
    // The unit circle divided into 8 sections
    if (x > 0 && y > 0 && x >= y) return 0 + f_atan(y / x);
    if (x > 0 && y > 0 && x < y) return R1 - f_atan(x / y);

    if (x < 0 && y > 0 && -x <= y) return R1 + f_atan(-x / y);
    if (x < 0 && y > 0 && -x > y) return R2 - f_atan(-y / x);

    if (x < 0 && y < 0 && -x >= -y) return -R2 + f_atan(y / x);
    if (x < 0 && y < 0 && -x < -y) return -R1 - f_atan(x / y);

    if (x > 0 && y < 0 && x <= -y) return -R1 + f_atan(-x / y);
    if (x > 0 && y < 0 && x > -y) return 0 - f_atan(-y / x);
    return 0;
}

// https://gist.github.com/petrsm/079de9396d63e00d5994a7cc936ae9c7
// seems pretty accurate
static f32 f_pow2(f32 x) {
    // 2^x = 2^i * 2^f
    i32 pi = f_floor(x); // integer part
    f32 pf = x - pi;     // fractional part, 0-1
    assert((f32)pi <= x, "pow2 error");
    assert(pi >= -127 && pi <= 128, "pow2 error");
    assert(pf >= 0 && pf <= 1, "pow2 error");
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
    if (v <= 0) return 0;

    // Initial guess
    union {
        f32 f;
        u32 i;
    } conv = {.f = v};
    conv.i = 0x5f3759df - (conv.i >> 1);

    // Newton method
    // > y = 1/sqrt(x)
    //
    // > f(y)  = 1/yy - x
    // > f'(y) = -2/yyy
    // > N(y) = y - f(y)/f'(y)
    // > N(y) = y*(1.5 - 0.5*xyy)
    f32 y = conv.f;
    y *= 1.5f - v * 0.5f * y * y;
    y *= 1.5f - v * 0.5f * y * y;
    return conv.f;
}

static f32 f_sqrt(f32 x) {
    if (x <= 0) return 0;
    // Newton method
    // > y = sqrt(x)
    // > yy - x = 0
    //
    // > f(y)  = yy - x
    // > f'(y) = 2y
    // > N(y) = y - f(y)/f'(y)
    // > N(y) = y - (yy-x)/2y
    // > N(y) = y - yy/2y + x/2y
    // > N(y) = 0.5*(y + x/y)
    f32 y = 1; // Starting at 1 (allows constant folding of first operation)
    y = 0.5 * (y + x / y);
    y = 0.5 * (y + x / y);
    y = 0.5 * (y + x / y);
    y = 0.5 * (y + x / y);
    return y;
}

static f32 f_acos(f32 x) {
    return f_atan2(f_sqrt((1.0 + x) * (1.0 - x)), x);
}

static f32 f_asin(f32 x) {
    return f_atan2(x, f_sqrt((1.0 + x) * (1.0 - x)));
}

static f32 f_remap(f32 x, f32 xl, f32 xh, f32 yl, f32 yh) {
    if (x <= xl) return yl;
    if (x >= xh) return yh;
    f32 y = (x - xl) / (xh - xl);
    return y * (yh - yl) + yl;
}

// Animate (TODO: fix)
static f32 animate(f32 x, f32 dt) {
    x += dt;
    return f_clamp(x, 0, 1);
}

static bool animate2(f32 *x, f32 dt) {
    *x -= dt;
    return *x <= 0;
}

static void animate_exp(f32 *value, f32 target, f32 dt) {
    *value += (target - *value) * dt;
}

static bool animate_lin(f32 *value, f32 target, f32 dt) {
    if (*value > target + dt) {
        *value -= dt;
        return false;
    } else if (*value < target - dt) {
        *value += dt;
        return false;
    } else {
        *value = target;
        return true;
    }
}

static bool is_near(f32 x, f32 y) {
    f32 eps = 0.001;
    return x > y - eps && x < y + eps;
}
