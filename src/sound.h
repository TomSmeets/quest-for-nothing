// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// sound.h - Audio synthesizer
#pragma once
#include "inc.h"
#include "math.h"

// Tracks a number of osscilators
struct Sound {
    f32 dt;

    u32 beat;
    f32 bpm;
    f32 beat_time;

    u32 index;
    f32 vars[64];
};

// ==== Base ====
static void snd_begin(Sound *snd, f32 dt, f32 bpm) {
    snd->dt  = dt;
    snd->bpm = bpm;
    snd->index = 0;

    // Calculate beat number
    snd->beat_time = snd->beat_time + dt*bpm / 60;
    if(snd->beat_time > 1) {
        snd->beat_time -= 1;
        snd->beat++;
    }
}

// Access a persistent sound variable
static f32 *snd_var(Sound *snd) {
    assert(snd->index < array_count(snd->vars));
    return snd->vars + snd->index++;
}

// A simple ramp from 0 to 1 with the given frequency
static f32 snd_ramp(Sound *snd, f32 freq) {
    f32 *v = snd_var(snd);
    f32 ret = *v;
    *v = f_fract(*v + snd->dt*freq);
    return ret;
}


// ==== Generators ====

// Sine wave
static f32 snd_sin(Sound *snd, f32 freq) {
    return f_sin(snd_ramp(snd, freq)*R4);
}

// Square wave
static f32 snd_square(Sound *snd, f32 freq, f32 mod) {
    f32 v = snd_ramp(snd, freq)*2;
    return v > 1 + mod ? 1 : -1;
}

// Triangle wave
static f32 snd_tri(Sound *snd, f32 freq) {
    f32 v = snd_ramp(snd, freq)*4;
    if(v > 3) return (v - 3) - 1;
    if(v > 1) return 1 - (v - 1);
    return v;
}

// ==== Effects ====
static f32 snd_compress(f32 v, f32 amount) {
    return f_clamp(v*amount, -1, 1);
}

static f32 snd_cut(f32 v, f32 amount) {
    if(v > 0) return f_max(v - amount, 0);
    if(v < 0) return f_min(v + amount, 0);
    return 0;
}

// ==== Filters ====
// https://karmafx.net/docs/karmafx_digitalfilters.pdf
// https://en.wikipedia.org/wiki/Low-pass_filter
