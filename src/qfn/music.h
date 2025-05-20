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
    f32 t;
    u32 beat;

    f32 f1;
    f32 f2;
    f32 f3;
    f32 f4;
    f32 f5;
    f32 f6;

    f32 v1;
    f32 v2;
    f32 v3;
    f32 v4;
    f32 v5;
    f32 v6;
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

    u32 beat = music->beat++;
    f32 beat_per_second = 2 * 60.0f / 60.0f;
    f32 dt = 1.0f / beat_per_second;
    music->sleep_time += dt;

    f32 v = 0.0f;
    v += f_sin2pi(music->t*1)  / 1 * music->v1;
    v += f_sin2pi(music->t*2)  / 2 * music->v2;
    v += f_sin2pi(music->t*3)  / 4 * music->v3;
    v += f_sin2pi(music->t*4)  / 8 * music->v4;
    v += f_sin2pi(music->t*5) / 16 * music->v5;
    v += f_sin2pi(music->t*6) / 32 * music->v6;
    i32 note = v;

    music->t += 1.0f / 16.0f;
    if(music->t > 1.0f) {
        music->t -= 1.0f;
        music->v1 = rand_f32(G->rand, 0, 1);
        music->v2 = rand_f32(G->rand, 0, 1);
        music->v3 = rand_f32(G->rand, 0, 1);
        music->v4 = rand_f32(G->rand, 0, 1);
        music->v5 = rand_f32(G->rand, 0, 1);
        music->v6 = rand_f32(G->rand, 0, 1);
    }

    Voice snd = {};
    snd.freq = music_note_to_freq(3 * 7 + note);
    snd.time = 0;
    snd.kind = 2;
    // gfx_audio_lock(eng->gfx);
    // audio_play(eng->audio, snd);
    // gfx_audio_unlock(eng->gfx);
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
