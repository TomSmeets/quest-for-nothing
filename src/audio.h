// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// audio.h - Immediate mode audio synthesis and digital filters
#pragma once
#include "id.h"
#include "math.h"
#include "rand.h"
#include "sound.h"
#include "std.h"
#include "types.h"

// Note to frequency relative to Middle C
static f32 sound_note_to_freq(i32 note) {
    return 440.0f * f_pow2((f32)(note + 3) / 12.0f);
}

// Sound System

// References:
//   How to disscet a Melody: https://www.youtube.com/watch?v=PZ5POrP-NMw
//   - Melodic Embellishments + Melodic Reduction
//   Learn every chord and chord symbol: https://www.youtube.com/watch?v=CyNiY1jzOuQ
//   - Chords vs Melody
// Inspiration
//   - https://www.youtube.com/watch?v=J_FCCvNzbiY
typedef struct {
    f32 dt;
    Random rand;

    u32 sound_count;
    Sound sounds[64];
} Audio;

static Audio *audio_new(Memory *mem) {
    return mem_struct(mem, Audio);
}

static void audio_play(Audio *audio, Sound new_sound) {
    sound_reset(&new_sound);

    for (u32 i = 0; i < array_count(audio->sounds); ++i) {
        Sound *sound = audio->sounds + i;

        // Find an empty slot
        if (sound->playing) continue;

        *sound = new_sound;
        fmt_su(OS_FMT, "Playing Sound in slot ", i, "\n");
        return;
    }

    fmt_s(OS_FMT, "Could not play sound\n");
}
