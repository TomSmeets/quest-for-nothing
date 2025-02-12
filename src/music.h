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

static void music_init(Music *music) {
    // u32 notes[3][] = {
    //     { 1, 1, 2, 2 }, // Note
    //     { 1, 1, 1, 1 }, // Length
    // }
    // u32 notes[] = {}
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

        #if 0
        {
            Sound snd = {};
            snd.freq = music_note_to_freq(music->note + 4 * 7);
            snd.src_a.freq = 1;
            snd.src_a.volume = .25;
            snd.src_a.attack_time = 0.1;
            snd.src_a.release_time = dt * 2;

            snd.src_b.freq = 1.0f;
            snd.src_b.volume = 1.0;
            snd.src_b.attack_time = 0.2;
            snd.src_b.release_time = dt * 2;

            snd.src_c.freq = 1;
            snd.src_c.volume = 0;
            snd.src_c.attack_time = 0.1;
            snd.src_c.release_time = dt * 2;
            snd.time = time;
            audio_play(eng->audio, snd);
        }

        {
            Sound snd = {};
            snd.freq = music_note_to_freq(music->note + 3 * 7);
            snd.src_a.freq = 1;
            snd.src_a.volume = .125;
            snd.src_a.attack_time = 0.2;
            snd.src_a.release_time = dt;

            snd.src_b.freq = 1.0f;
            snd.src_b.volume = 8.0;
            snd.src_b.attack_time = 0.2;
            snd.src_b.release_time = dt;

            snd.src_c.freq = 1;
            snd.src_c.volume = 0;
            snd.src_c.attack_time = 0.5;
            snd.src_c.release_time = dt;
            snd.time = time;
            audio_play(eng->audio, snd);
        }

        #endif
        time -= 1.0f * dt;

        if (rand_u32(&eng->rng) % 2 == 0) {
            music->note += 1;
        } else {
            music->note -= 1;
        }

        if (music->note > 7) music->note = 6;
        if (music->note < -7) music->note = -6;
    }
}

#if 0
typedef enum {
    Sound_Step,
} Sound_Type;

typedef struct {
    f32 freq;
    f32 time;

    f32 attack;
    f32 release;
    f32 sustain;
    f32 decay;

    u32 phase_count;
    f32 phase[64];

    // 3d sound params
    v3 pos;
    v3 vel;
} Sound;

typedef struct {
    Random rand;

    // Immediate mode data
    u32 phase_count;
    f32 phase[64];
} Audio;

static f32 audio_step(Audio *ctx) {
    f32 v = 0;
    f32 t = 0;
    for(u32 i = 0; i < ctx->sound_count; ++i) {
        Sound *snd = ctx->sounds + i;
        f32 a = audio_sine(t, 440, 0);
        f32 b = audio_sine(t,  10, 0);
        audio_noise(ctx->rand)
        v += a * (1 + b * 0.5);
    }
    f32 r = audio_reverb(v);
    return r;
}

static void audio_lock():
static void audio_unlock(Audio *audio)
#endif
