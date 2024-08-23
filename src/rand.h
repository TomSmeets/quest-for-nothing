// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// rand.h - Fast and random number generator
#pragma once
#include "types.h"
#include "vec.h"

typedef struct {
    u64 seed;
} Random;

static u32 rand_xorshift(Random *rand) {
    rand->seed ^= (rand->seed << 21);
    rand->seed ^= (rand->seed >> 35);
    rand->seed ^= (rand->seed << 4);
    return (u32)rand->seed;
}

static u32 rand_lcg(Random *rand) {
    rand->seed = 6364136223846793005ULL * rand->seed + 1;
    u32 xorshifted = ((rand->seed >> 18u) ^ rand->seed) >> 27u;
    u32 rot = rand->seed >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

// Generate a new random 32 bit number
static u32 rand_u32(Random *rand) {
    return rand_lcg(rand);
}

// Fork the random state
static Random rand_fork(Random *rand) {
    return (Random){rand_u32(rand) + 1};
}

// Random f32 in range [0, 1]
static f32 rand_f32(Random *rand) {
    return (f32)rand_u32(rand) / (f32)U32_MAX;
}

// Random f32 in range [0, 1]
static f32 rand_f32_range(Random *rand, f32 min, f32 max) {
    return rand_f32(rand) * (max - min) + min;
}

// Random f32 in range [0, 1]
static f32 rand_u32_range(Random *rand, f32 min, f32 max) {
    return rand_f32_range(rand, min, max);
}

// Random f32 in range [-1, 1]
static f32 rand_f32_signed(Random *rand) {
    return rand_f32(rand) * 2.0f - 1.0f;
}

static v4 rand_color(Random *rng) {
    v4 ret;
    ret.x = rand_f32(rng);
    ret.y = rand_f32(rng);
    ret.z = rand_f32(rng);
    ret.w = 1;
    return ret;
}
