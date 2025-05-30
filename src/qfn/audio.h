// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// audio.h - Immediate mode audio synthesis and digital filters
#pragma once
#include "math.h"
#include "mem.h"
#include "rand.h"
#include "sound.h"
#include "sound_filter.h"
#include "types.h"

// Sound System

// References:
//   How to disscet a Melody: https://www.youtube.com/watch?v=PZ5POrP-NMw
//   - Melodic Embellishments + Melodic Reduction
//   Learn every chord and chord symbol: https://www.youtube.com/watch?v=CyNiY1jzOuQ
//   - Chords vs Melody
// Inspiration
//   - https://www.youtube.com/watch?v=J_FCCvNzbiY
typedef struct {
    Sound snd;
} Audio;

static Audio *audio_new(Memory *mem) {
    return mem_struct(mem, Audio);
}

static v2 audio_sample(Audio *audio) {
    sound_begin(&audio->snd);
    return sound_music(&audio->snd);
}
