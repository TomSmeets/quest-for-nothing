// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// rand.h - Fast and random number generator
#pragma once
#include "types.h"
#include "vec.h"

typedef struct Rand Rand;
typedef struct Rand Random;

static Rand rand_new(u64 seed);
static Rand rand_fork(Rand *rand);

// Random number [0, 2^32)
static u32 rand_next(Rand *rand);

// Random number in range [min, max)
static u32 rand_u32(Rand *rand, u32 min, u32 max);
static i32 rand_i32(Rand *rand, i32 min, i32 max);
static f32 rand_f32(Rand *rand, f32 min, f32 max);

struct Rand { u64 seed; };

static Rand rand_new(u64 seed) {
    return (Rand){seed};
}

static u32 rand_next(Rand *rand) {
    rand->seed = 6364136223846793005ULL * rand->seed + 1;
    u32 xorshifted = ((rand->seed >> 18u) ^ rand->seed) >> 27u;
    u32 rot = rand->seed >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

static Rand rand_fork(Rand *rand) {
    return rand_new(rand_next(rand) + 1);
}

static f32 rand_f32(Rand *rand, f32 min, f32 max) {
    return (f32)rand_next(rand) / (f32)0x100000000 * (max - min) + min;
}

static u32 rand_u32(Rand *rand, u32 min, u32 max) {
    return (u32) rand_f32(rand, min, max - 1);
}

static i32 rand_i32(Rand *rand, i32 min, i32 max) {
    u32 range = max - min;
    return (i32)rand_u32(rand, 0, range) + min;
}

static bool rand_choice(Rand *rand, f32 chance) {
    return rand_f32(rand, 0, 1) < chance;
}

static v3 rand_color(Rand *rand) {
    return (v3) {
        rand_f32(rand, 0, 1),
        rand_f32(rand, 0, 1),
        rand_f32(rand, 0, 1),
    };
}
