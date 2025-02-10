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
// https://www.musicdsp.org/en/latest/index.html
//

typedef struct Sound Sound;
struct Sound {
    // Duration
    f32 duration;

    // Base frequency
    f32 freq;

    // Implementation
    f32 (*algorithm)(Sound *);

    // Current sound runtime
    f32 time;

    // Sample detla time
    f32 dt;

    // For noise
    Random rand;

    // List of 'phase' values for sine waves.
    u32 phase_ix;
    f32 phase[64];
};

// Very simple envelope
static f32 sound_envelope(f32 time, f32 attack_time, f32 duration, f32 release_time) {
    f32 v_attack = f_step_duration(time, attack_time);
    f32 v_release = f_step_duration(time - duration - attack_time, release_time);
    return f_min(v_attack, 1 - v_release);
}

static f32 sound_sample(Sound *sound) {
    sound->phase_ix = 0;
    f32 sample = (*sound->algorithm)(sound);
    sound->time += sound->dt;
    return sample;
}

static void sound_var_begin(Sound *snd) {
    snd->phase_ix = 0;
}

static void sound_var_end(Sound *snd) {
    snd->time += snd->dt;
}

// Get a new persistent variable for this sample
static f32 *sound_var(Sound *sound) {
    if (sound->phase_ix >= array_count(sound->phase)) return 0;
    return sound->phase + sound->phase_ix++;
}

// Get a number of persistent variables for this sample
static f32 *sound_var_list(Sound *sound, u32 count) {
    if (sound->phase_ix + count > array_count(sound->phase)) return 0;
    f32 *ret = sound->phase + sound->phase_ix;
    sound->phase_ix += count;
    return ret;
}

// Generate linear ramp from [0, 1) at a given frequency
static f32 sound_ramp(Sound *sound, f32 freq, f32 phase) {
    // Get a phase value
    f32 *var = sound_var(sound);

    // Return current value (start at 0)
    f32 ret = f_fract(*var + phase);
    *var = f_fract(*var + sound->dt * freq);
    return ret;
}

// Saw wave (rising)
static f32 sound_saw(Sound *sound, f32 freq, f32 phase) {
    return sound_ramp(sound, freq, phase) * 2.0f - 1.0f;
}

// Pulse/square wave
static f32 sound_pulse(Sound *sound, f32 freq, f32 phase, f32 duty) {
    return sound_ramp(sound, freq, phase) < duty;
}

// Generate a pure sine wave at a given frequency
// range = [-1, 1]
static f32 sound_sine(Sound *sound, f32 freq, f32 phase) {
    return f_sin2pi(sound_ramp(sound, freq, phase));
}

static f32 sound_noise_white(Sound *sound) {
    return rand_f32_signed(&sound->rand);
}

static f32 sound_noise(Sound *sound, f32 freq, f32 duty) {
    // Current sample
    f32 *value = sound_var(sound);

    // Timer until next sample
    f32 *timer = sound_var(sound);

    // Increment timer
    *timer += sound->dt * freq;

    // Value becomes 0 after duty cycle
    if (*timer > duty) *value = 0.0f;

    // Timer triggered, get a new sample
    if (*timer >= 1.0f) {
        *value = rand_f32_signed(&sound->rand);
        *timer -= (i32)*timer;
    }

    return *value;
}

// Delay input by 'count' samples
static f32 sound_delay(Sound *sound, f32 input, u32 count) {
    f32 value = input;
    for (u32 i = 0; i < count; ++i) {
        f32 *prev = sound_var(sound);
        f32 tmp = *prev;
        *prev = value;
        value = tmp;
    }
    return value;
}

// Feedback signal
static f32 *sound_feedback(Sound *sound) {
    return sound_var(sound);
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

    f32 rc = 1.0f / (cutoff_freq * PI2);
    f32 f = sound->dt / (rc + sound->dt);

    // f and fb calculation
    f32 q = 0.9f;
    f32 fb = q + q / (1.0f - f);

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
