// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// rand.h - Fast and random number generator

#pragma once
#include "inc.h"
#include "vec.h"

// a fast procudural random number generator
struct rand_t {
    u64 seed;
};

static u32 rand_xorshift(rand_t *r) {
    r->seed ^= (r->seed << 21);
    r->seed ^= (r->seed >> 35);
    r->seed ^= (r->seed << 4);
    return (u32) r->seed;
}

static u32 rand_lcg(rand_t *r) {
    r->seed = 6364136223846793005ULL*r->seed + 1;
    u32 xorshifted = ((r->seed >> 18u) ^ r->seed) >> 27u;
    u32 rot = r->seed >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

static u32 rand_next(rand_t *r) {
    return rand_lcg(r);
}

static rand_t rand_split(rand_t *r) {
    return (rand_t){ rand_next(r) + 1 };
}

static f32 rand_f32(rand_t *r) {
    u32 max = U32_MAX;
    u32 v = rand_next(r);
    return (f32) v / (f32) max;
}

static f32 rand_f_signed(rand_t *r) {
    return rand_f32(r)*2.0 - 1.0;
}

static f32 rand_f32_signed(rand_t *r) {
    return rand_f32(r)*2.0 - 1.0;
}

static u32 rand_range(rand_t *r, u32 lim) {
#if 1
    u32 threshold = (-lim) % lim;
    for (;;) {
        u32 v = rand_next(r);
        if (v >= threshold)
            return v % lim;
    }
#else
    u64 max = U64_MAX - (U64_MAX % lim + 1) % lim + 1;
    
    for(;;) {
        u64 v = rand_next(r);
        if(v < max) return v % lim;
    }
#endif
}

static u32 rand_range2(rand_t *r, u32 min, u32 max) {
    assert(min < max);
    return rand_range(r, max - min) + min;
}

static i32 rand_signed_range(rand_t *r, u32 range) {
    return (i32) rand_range(r, range*2+1) - (i32) range;
}

static v3 rand_v2(rand_t *r) {
    f32 a = rand_f32(r)*R4;
    f32 d = rand_f32(r);
    return (v3){f_cos(a), f_sin(a)}*f_sqrt(d);
}
