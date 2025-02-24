// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// rand.h - Fast and random number generator
#pragma once
#include "types.h"
#include "vec.h"

typedef struct {
    u64 seed;
} Rand;

static Rand rand_new(u64 seed) {
    return (Rand){seed};
}

// Generae a random 32 bit number
// range: [0, U32_MAX]
static u32 rand_next(Rand *rand) {
    rand->seed = 6364136223846793005ULL * rand->seed + 1;
    u32 xorshifted = ((rand->seed >> 18u) ^ rand->seed) >> 27u;
    u32 rot = rand->seed >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

// Split this rng into two 'timelines'
static Rand rand_fork(Rand *rand) {
    return rand_new(rand_next(rand) + 1);
}

// Random float in range [min, max)
static f32 rand_f32(Rand *rand, f32 min, f32 max) {
    return (f32)rand_next(rand) / (f32)0x100000000 * (max - min) + min;
}

// Random unsinged in range [min, max)
static u32 rand_u32(Rand *rand, u32 min, u32 max) {
    return (u32)rand_f32(rand, min, max - 1);
}

// Random signed in range [min, max)
static i32 rand_i32(Rand *rand, i32 min, i32 max) {
    u32 range = max - min;
    return (i32)rand_u32(rand, 0, range) + min;
}

// Random choice with a given chance
// chance = 0 -> Never
// chance = 1 -> Always
static bool rand_choice(Rand *rand, f32 chance) {
    return rand_f32(rand, 0, 1) < chance;
}

// TODO: is not very good, remove
static v3 rand_color(Rand *rand) {
    return (v3){
        rand_f32(rand, 0, 1),
        rand_f32(rand, 0, 1),
        rand_f32(rand, 0, 1),
    };
}
