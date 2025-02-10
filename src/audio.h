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

#define MUSIC_C4 (7 * 4)
static f32 music_note_to_freq(u32 note) {
    assert(note <= 7 * 8, "only 8 supported ocatves");

    f32 octave_list[9] = {
        1.0f / 16.0f, // 0
        1.0f / 8.0f,  // 1
        1.0f / 4.0f,  // 2
        1.0f / 2.0f,  // 3
        1.0f,         // 4
        2.0f,         // 5
        4.0f,         // 6
        8.0f,         // 7
        16.0f,        // 8
    };

    // A - F
    f32 note_list[7] = {
        261.62556530059860, // C
        // 277.18263097687210, // C#
        293.66476791740760, // D
        // 311.12698372208090, // D#
        329.62755691286990, // E
        349.22823143300390, // F
        // 369.99442271163440, // F#
        391.99543598174927, // G
        // 415.30469757994510, // G#
        440.00000000000000, // A
        // 466.16376151808990, // A#
        493.88330125612410, // B
    };

    return octave_list[note / 7] * note_list[note % 7];
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

static void audio_play(Audio *audio, f32 (*algorithm)(Sound *), f32 duration, u32 note) {
    for (u32 i = 0; i < array_count(audio->sounds); ++i) {
        Sound *sound = audio->sounds + i;

        // Find an empty slot
        if (sound->algorithm) continue;

        audio_lock();
        *sound = (Sound){};
        sound->dt = 1.0 / AUDIO_SAMPLE_RATE;
        sound->rand = rand_fork(&audio->rand);
        sound->duration = duration;
        sound->algorithm = algorithm;
        sound->freq = music_note_to_freq(note);
        audio_unlock();
    }
    fmt_s(OS_FMT, "Could not play sound\n");
}
