// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// audio.h - Immediate mode audio synthesis and digital filters
#pragma once
#include "math.h"
#include "rand.h"
#include "std.h"
#include "types.h"

typedef struct {
    f32 dt;
    u32 wave_ix;
    f32 wave[64];
    Random rand;
} Audio;

static void audio_begin_sample(Audio *audio, f32 dt) {
    audio->wave_ix = 0;
    audio->dt = dt;
}

// Get a new persistent variable for this sample
static f32 *audio_var(Audio *audio) {
    if (audio->wave_ix >= array_count(audio->wave)) return 0;
    return audio->wave + audio->wave_ix++;
}

// Generate linear ramp from [0, 1) at a given frequency
static f32 audio_ramp(Audio *audio, f32 freq) {
    f32 *var = audio_var(audio);
    if (!var) return 0;
    f32 sample = *var;
    *var += audio->dt * freq;
    *var -= (i32)*var;
    return sample;
}

static f32 audio_saw(Audio *audio, f32 freq) {
    return audio_ramp(audio, freq) * 2.0f - 1.0f;
}

static f32 audio_pulse(Audio *audio, f32 freq, f32 duty) {
    return audio_ramp(audio, freq) < duty;
}

// Generate a pure sine wave at a given frequency
// range = [-1, 1]
static f32 audio_sine(Audio *audio, f32 freq) {
    return f_sin2pi(audio_ramp(audio, freq));
}

// Filter the incoming samples at a given cutoff frequency.
static f32 audio_filter(Audio *audio, f32 cutoff_freq, f32 sample) {
    f32 *buf0 = audio_var(audio);
    f32 *buf1 = audio_var(audio);

    f32 rc = 1.0 / (cutoff_freq * PI2);
    f32 f = audio->dt / (rc + audio->dt);

    // f and fb calculation
    f32 q = 0.5;
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

    return lp;
}

static f32 audio_noise_white(Audio *audio) {
    return rand_f32_signed(&audio->rand);
}

static f32 audio_noise_freq(Audio *audio, f32 freq, f32 duty) {
    f32 *var = audio_var(audio);
    f32 *last = audio_var(audio);
    *var += audio->dt * freq;
    if (*var > duty) {
        *last = 0.0f;
    }
    if (*var > 1.0f) {
        *last = rand_f32_signed(&audio->rand);
    }
    *var -= (i32)*var;
    return *last;
}

static f32 audio_smooth_bool(Audio *audio, f32 speed, bool b) {
    f32 *value = audio_var(audio);
    f32 target = b ? 1.0f : 0.0f;
    f32 ret = *value;
    *value += (target - *value) * audio->dt * speed;
    return ret;
}
