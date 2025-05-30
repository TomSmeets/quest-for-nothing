// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sound_var.h - Immediate mode sound synthesis
#pragma once
#include "math.h"
#include "rand.h"
#include "types.h"

#define SOUND_SAMPLE_RATE 48000
#define SOUND_DT (1.0f / SOUND_SAMPLE_RATE)

// ==== SOUND TYPE ====
typedef struct {
    // Is this the very first sample?
    u32 sample;

    // For noise
    Rand rand;

    // Immediate mode memory
    u32 index;
    u32 data[1024 * 32];
} Sound;

// Start producing a new sound sample
static void sound_begin(Sound *snd) {
    snd->index = 0;
    snd->sample++;
}

static void sound_reset(Sound *snd) {
    *snd = (Sound){};
}

// Get a new persistent variable for this sample
static void *sound_push(Sound *sound, u32 size) {
    // Align up to 4 bytes
    size = (size + 3) & ~3;
    assert0(sound->index + size <= sizeof(sound->data));
    void *ret = (u8 *)sound->data + sound->index;
    assert0((u64)ret % 4 == 0);
    sound->index += size;
    return ret;
}

#define sound_var(sound, type) ((type *)sound_push((sound), sizeof(type)))
#define sound_vars(sound, type, count) ((type *)sound_push((sound), sizeof(type) * (count)))
