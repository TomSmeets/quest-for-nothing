// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// music.h - Music generation
#pragma once
#include "audio.h"
#include "engine.h"
#include "sound.h"
#include "types.h"

// 4/4 timing
// | 0123 | 4567 | 89ab | cdef |

typedef struct {
    f32 sleep_time;
} Music;

typedef struct {
    f32 note;
    f32 duration;
    f32 time;
} Note;

// C D E F G A B C
// note, rest
//
// 0 0 0

static void music_init(Music *music) {
}

static void music_play(Music *music, Engine *eng) {
    music->sleep_time -= eng->dt;
    if (music->sleep_time > 0) return;

    u32 beat_count = 4;
    f32 beat_per_second = 70.0f / 60.0f;
    f32 dt = 1.0f / beat_per_second;
    music->sleep_time += dt * beat_count;

    // Enqueue new music
    Sound snd = {};
    snd.src_a.freq = 1;
    snd.src_a.volume = .5;
    snd.src_a.attack_time = 0.0;
    snd.src_a.release_time = dt;

    snd.src_b.freq = 1;
    snd.src_b.volume = 2;
    snd.src_b.attack_time = 0.0;
    snd.src_b.release_time = dt;

    for (u32 i = 0; i < beat_count; ++i) {
        fmt_s(OS_FMT, "Submitting beats\n");
        snd.freq = sound_note_to_freq(i % 4) - 12 * 2;
        audio_play(eng->audio, snd);
        snd.time -= 1.0f * dt;
    }
}
