// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sound_osc.h - Sound Signal sources
#pragma once
#include "sound_var.h"

// Generate linear ramp from [0, 1) at a given frequency and phase offset
static f32 sound_phase(Sound *sound, f32 freq, f32 offset) {
    // Get a phase value
    f32 *phase = sound_var(sound, f32);

    // Compute current phase with offset
    f32 out = f_fract(*phase + offset);

    // Compute next variable
    *phase = f_fract(*phase + SOUND_DT * freq);

    return out;
}

// Saw wave (rising)
static f32 sound_saw(Sound *sound, f32 freq, f32 offset) {
    return sound_phase(sound, freq, offset) * 2.0f - 1.0f;
}

// Pulse wave
// on for duty% of the time
// off for the rest of the time
// duty = 0.5 -> 50% on then 50% off
// duty = 0.0 -> only low
// duty = 1.0 -> only high
static f32 sound_pulse(Sound *sound, f32 freq, f32 offset, f32 duty) {
    return sound_phase(sound, freq, offset) < duty ? 1 : -1;
}

// Generate a pure sine wave at a given frequency
// range = [-1, 1]
static f32 sound_sine(Sound *sound, f32 freq, f32 offset) {
    return f_sin2pi(sound_phase(sound, freq, offset));
}

static f32 sound_triangle(Sound *sound, f32 freq, f32 offset) {
    f32 v = sound_phase(sound, freq, offset);
    return f_min(v * 4 - 1, 3 - v * 4);
}

// Static white noise
static f32 sound_noise_white(Sound *sound) {
    return rand_f32(&sound->rand, -1, 1);
}

typedef struct {
    f32 phase;
    bool trigger;
    u32 index;
} Clock;

static Clock sound_clock(Sound *sound, f32 freq, u32 cycle) {
    f32 *value = sound_var(sound, f32);
    u32 *index = sound_var(sound, u32);

    *value += freq * SOUND_DT;
    bool trigger = false;
    while(*value >= 1.0f) {
        *value -= 1.0;
        *index += 1;
        *index %= cycle;
        trigger = true;
    }

    Clock ret = {};
    ret.trigger = trigger;
    ret.phase = *value;
    ret.index = *index;
    return ret;
}

// Noise at a given frequency
// Kind of like sound_pulse with random values
static f32 sound_noise_freq(Sound *sound, f32 freq, f32 duty) {
    // Current sample
    f32 *value = sound_var(sound, f32);
    Clock clk = sound_clock(sound, freq, 1);
    if (clk.trigger) *value = rand_f32(&sound->rand, -1, 1);
    f32 ret = clk.phase < duty ? *value : 0.0;
    return ret;
}
