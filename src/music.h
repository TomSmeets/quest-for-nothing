// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// music.h - Everything to do with music synthesis
#pragma once
#include "audio.h"
#include "engine.h"
#include "sound.h"
#include "types.h"

// 4/4 timing
// | 0123 | 4567 | 89ab | cdef |
typedef struct {
    f32 sleep_time;
    i32 note;
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

// Note to freq,
// starts at C0..
// Each octave is 7 notes
// C4 = 7*4
#define MUSIC_C4 (7*4)
static f32 music_note_to_freq(u32 note) {
    assert(note <= 7 * 8, "only 8 supported ocatves");

    f32 octave_list[9] = {
        1.0f / 16.0f, // 0
        1.0f / 8.0f, // 1
        1.0f / 4.0f, // 2
        1.0f / 2.0f, // 3
        1.0f, // 4
        2.0f, // 5
        4.0f, // 6
        8.0f, // 7
        16.0f, // 8
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

static void music_init(Music *music) {
    // u32 notes[3][] = {
    //     { 1, 1, 2, 2 }, // Note
    //     { 1, 1, 1, 1 }, // Length
    // }
    // u32 notes[] = {}
}

static bool sound(Sound_Vars *snd) {
    f32 a = sound_saw(snd, snd->freq);
    f32 b = sound_saw(snd, snd->freq);
}


static void music_play(Music *music, Engine *eng) {
    music->sleep_time -= eng->dt;
    if (music->sleep_time > 0) return;

    u32 beat_count = 4;
    f32 beat_per_second = 70.0f / 60.0f;
    f32 dt = 1.0f / beat_per_second;
    music->sleep_time += dt * beat_count;

    // Enqueue new music

    f32 time = 0;
    for (u32 i = 0; i < beat_count; ++i) {
        fmt_s(OS_FMT, "Submitting beats\n");

        {
            Sound snd = {};
            snd.freq = music_note_to_freq(music->note + 3 * 7);
            snd.src_a.freq = 1;
            snd.src_a.volume = 1;
            snd.src_a.attack_time = 0.1;
            snd.src_a.duration = dt;
            snd.src_a.release_time = dt;

            snd.src_b.freq = .5;
            snd.src_b.volume = .5;
            snd.src_b.attack_time = 0.1;
            snd.src_b.duration = dt;
            snd.src_b.release_time = dt;

            snd.time = time;
            audio_play(eng->audio, snd);
        }

        time -= 1.0f * dt;

        if (rand_u32(&eng->rng) % 2 == 0) {
            music->note += 1;
        } else {
            music->note -= 1;
        }

        if(music->note > 7) music->note = 6;
        if(music->note < -7) music->note = -6;
    }
}
