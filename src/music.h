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

static void music_init(Music *music) {
    // u32 notes[3][] = {
    //     { 1, 1, 2, 2 }, // Note
    //     { 1, 1, 1, 1 }, // Length
    // }
    // u32 notes[] = {}
}

static f32 music_algorithm(Sound *snd) {
    f32 volume = sound_envelope(snd->time, 0.1, snd->duration, 0.1);

    f32 b = sound_sine(snd, snd->freq, 0);
    f32 a = sound_sine(snd, snd->freq * (1 + b), 0);

    if (snd->time < 0) return 0;

    if (volume <= 0) {
        snd->algorithm = 0;
        return 0;
    }

    return a * volume;
}

static void music_play(Music *music, Engine *eng) {
    music->sleep_time -= eng->dt;
    if (music->sleep_time > 0) return;

    u32 beat_count = 1;
    f32 beat_per_second = 70.0f / 60.0f;
    f32 dt = 1.0f / beat_per_second;
    music->sleep_time += dt * beat_count;

    // Enqueue new music

    f32 time = 0;
    for (u32 i = 0; i < beat_count; ++i) {
        fmt_s(OS_FMT, "Submitting beats\n");

        Sound *snd = audio_play(eng->audio);
        snd->duration = dt;
        snd->freq = music_note_to_freq(music->note + 3 * 7);
        snd->time = time;
        snd->algorithm = music_algorithm;
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
