// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sound.h - Immediate mode sound synthesis and digital filters
#pragma once
#include "gfx.h"
#include "id.h"
#include "math.h"
#include "rand.h"
#include "std.h"
#include "types.h"

// https://www.eetimes.com/making-sounds-with-analogue-electronics-part-1-before-the-synthesizer/
// FM: https://www.youtube.com/watch?v=DD0UpZ5uGAo

typedef struct {
    // Runtime
    f32 time;
    f32 dt;

    // List of 'phase' values for sine waves.
    u32 phase_ix;
    f32 phase[16];

    // For noise
    Random rand;
} Sound;

static f32 sound_adsr(Sound *snd, f32 t_attack, f32 t_decay, f32 t_sustain, f32 t_release, bool *done) {
    f32 time = snd->time;

    f32 sustain_volume = 0.8;

    f32 value = 0.0f;
    value += f_step_duration(time, t_attack);
    time -= t_attack;
    value -= f_step_duration(time, t_decay) * (1.0f - sustain_volume);
    time -= t_decay;
    time -= t_sustain;
    value -= f_step_duration(time, t_release) * sustain_volume;
    time -= t_release;

    // Signal done
    if (done && time >= 0) *done = true;
    return value;
}

static void sound_begin(Sound *snd) {
    snd->phase_ix = 0;
}

static void sound_end(Sound *snd) {
    snd->time += snd->dt;
}

// Get a new persistent variable for this sample
static f32 *sound_var(Sound *sound) {
    assert(sound->phase_ix < array_count(sound->phase), "Out of sound memory");
    return sound->phase + sound->phase_ix++;
}

typedef struct {
    f32 low_pass;
    f32 band_pass;
    f32 high_pass;
} Sound_Filter_Result;

// Filter the incoming samples at a given cutoff frequency.
static Sound_Filter_Result sound_filter(Sound *sound, f32 cutoff_freq, f32 sample) {
    f32 *var0 = sound_var(sound);
    f32 *var1 = sound_var(sound);

    f32 rc = 1.0 / (cutoff_freq * PI2);
    f32 f = sound->dt / (rc + sound->dt);

    // f and fb calculation
    f32 q = 0.9;
    f32 fb = q + q / (1.0 - f);

    // loop

    // High Pass Filter
    f32 hp = sample - *var0;

    // Band Pass Filter
    f32 bp = *var0 - *var1;

    *var0 += f * (hp + fb * bp);
    *var1 += f * (*var0 - *var1);

    // Low Pass Filter
    f32 lp = *var1;

    return (Sound_Filter_Result){lp, bp, hp};
}

// Generate linear ramp from [0, 1) at a given frequency and phase offset
static f32 sound_ramp(Sound *sound, f32 freq, f32 offset) {
    // Get a phase value
    f32 *phase = sound_var(sound);

    // Compute current phase with offset
    f32 out = f_fract(*phase + offset);

    // Compute next variable
    *phase = f_fract(*phase + sound->dt * freq);
    return f_fract(out + offset);
}

// Saw wave (rising)
static f32 sound_saw(Sound *sound, f32 freq, f32 offset) {
    return sound_ramp(sound, freq, offset) * 2.0f - 1.0f;
}

// Pulse wave
// on for duty% of the time
// off for the rest of the time
// duty = 0.5 -> 50% on then 50% off
// duty = 0.0 -> only low
// duty = 1.0 -> only high
static f32 sound_pulse(Sound *sound, f32 freq, f32 offset, f32 duty) {
    return sound_ramp(sound, freq, offset) < duty ? 1 : -1;
}

// Generate a pure sine wave at a given frequency
// range = [-1, 1]
static f32 sound_sine(Sound *sound, f32 freq, f32 offset) {
    return f_sin2pi(sound_ramp(sound, freq, offset));
}

// Static white noise
static f32 sound_noise_white(Sound *sound) {
    return rand_f32_signed(&sound->rand);
}

// Noise at a given frequency
// Kind of like sound_pulse with random values
static f32 sound_noise_freq(Sound *sound, f32 freq, f32 duty) {
    // Current sample
    f32 *value = sound_var(sound);

    // Timer until next sample
    f32 *phase = sound_var(sound);

    f32 ret = *phase < duty ? *value : 0.0;

    // Increment phase
    *phase += sound->dt * freq;

    // Reached next cycle
    if (*phase >= 1.0) {
        *value = rand_f32_signed(&sound->rand);
        *phase -= (i32)*phase;
    }

    return ret;
}
