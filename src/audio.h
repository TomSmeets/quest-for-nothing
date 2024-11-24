// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// audio.h - Immediate mode audio synthesis and digital filters
#pragma once
#include "math.h"
#include "rand.h"
#include "std.h"
#include "types.h"

// A single sound
typedef struct {
    // For 3d sounds
    v3 pos;

    // Time increment in seconds
    f32 dt;

    // List of 'phase' values for sine waves.
    u32 phase_ix;
    f32 phase[16];

    // RNG for noise
    Random rand;
} Sound;

// Sound System
typedef struct {
    // RNG to seed sound rngs
    Random rand;

    u32 sound_count;
    Sound sounds[16];
} Audio;

static void audio_play(Audio *audio, u32 kind) {
    Sound *sound = audio->sounds + audio->sound_count++;
    sound->rand = rand_fork(&audio->rand);
}

static void audio_remove(Audio *audio, u32 sound) {
    // Swap remove sound
    u32 last_ix = --audio->sound_count;
    audio->sounds[sound] = audio->sounds[last_ix];
    audio->sounds[last_ix] = (Sound){0};
}

static void sound_begin_sample(Sound *sound, f32 dt) {
    sound->phase_ix = 0;
    sound->dt = dt;
}

// Get a new persistent variable for this sample
static f32 *sound_var(Sound *sound) {
    if (sound->phase_ix >= array_count(sound->phase)) return 0;
    return sound->phase + sound->phase_ix++;
}

// Generate linear ramp from [0, 1) at a given frequency
static f32 sound_ramp(Sound *sound, f32 freq) {
    f32 *var = sound_var(sound);
    if (!var) return 0;
    f32 sample = *var;
    *var += sound->dt * freq;
    *var -= (i32)*var;
    return sample;
}

static f32 sound_saw(Sound *sound, f32 freq) {
    return sound_ramp(sound, freq) * 2.0f - 1.0f;
}

static f32 sound_pulse(Sound *sound, f32 freq, f32 duty) {
    return sound_ramp(sound, freq) < duty;
}

// Generate a pure sine wave at a given frequency
// range = [-1, 1]
static f32 sound_sine(Sound *sound, f32 freq) {
    return f_sin2pi(sound_ramp(sound, freq));
}

typedef struct {
    f32 low_pass;
    f32 band_pass;
    f32 high_pass;
} Sound_Filter_Result;

// Filter the incoming samples at a given cutoff frequency.
static Sound_Filter_Result sound_filter(Sound *sound, f32 cutoff_freq, f32 sample) {
    f32 *buf0 = sound_var(sound);
    f32 *buf1 = sound_var(sound);

    f32 rc = 1.0 / (cutoff_freq * PI2);
    f32 f = sound->dt / (rc + sound->dt);

    // f and fb calculation
    f32 q = 0.9;
    f32 fb = q + q / (1.0 - f);

    // loop

    // High Pass Filter
    f32 hp = sample - *buf0;

    // Band Pass Filter
    f32 bp = *buf0 - *buf1;

    *buf0 += f * (hp + fb * bp);
    *buf1 += f * (*buf0 - *buf1);

    // Low Pass Filter
    f32 lp = *buf1;

    return (Sound_Filter_Result){lp, bp, hp};
}

static f32 sound_noise_white(Sound *sound) {
    return rand_f32_signed(&sound->rand);
}

static f32 sound_noise_freq(Sound *sound, f32 freq, f32 duty) {
    f32 *var = sound_var(sound);
    f32 *last = sound_var(sound);
    *var += sound->dt * freq;
    if (*var > duty) {
        *last = 0.0f;
    }
    if (*var > 1.0f) {
        *last = rand_f32_signed(&sound->rand);
    }
    *var -= (i32)*var;
    return *last;
}

static f32 sound_smooth_bool(Sound *sound, f32 speed, bool b) {
    f32 *value = sound_var(sound);
    f32 target = b ? 1.0f : 0.0f;
    f32 ret = *value;
    *value += (target - *value) * sound->dt * speed;
    return ret;
}
