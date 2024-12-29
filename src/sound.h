// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// sound.h - Immediate mode sound synthesis and digital filters
#pragma once
#include "id.h"
#include "math.h"
#include "os_gfx.h"
#include "rand.h"
#include "sound.h"
#include "std.h"
#include "types.h"

// https://www.eetimes.com/making-sounds-with-analogue-electronics-part-1-before-the-synthesizer/

typedef struct {
    bool done;
    f32 volume;
} Sound_Result;

// Sound Envelope
typedef struct {
    f32 attack;
    f32 decay;
    f32 sustain;
    f32 release;
} Sound_Envelope;

// Envelope
// Attack to 2
// Decay to 1
// Sustain 1
// Relase to 0
//
//
//  /\___
// /     \\
// a d s r
static Sound_Result sound_envelope(Sound_Envelope *env, f32 time) {
    f32 v = 0.0;
    f32 t = time;

    f32 peak = 0.2;
    f32 volume = 0.8;

    v += f_step_duration(t, env->attack) * (volume + peak);
    t -= env->attack;

    v -= f_step_duration(t, env->decay) * peak;
    t -= env->decay;

    t -= env->sustain;

    v -= f_step_duration(t, env->release) * volume;
    t -= env->release;
    return (Sound_Result){.done = t >= 0, .volume = v};
}

typedef struct {
    // Runtime
    f32 time;
    f32 dt;

    // List of 'phase' values for sine waves.
    u32 phase_ix;
    f32 phase[16];

    // For noise
    Random rand;
} Sound_Vars;

static void sound_var_begin(Sound_Vars *snd) {
    snd->phase_ix = 0;
}

static void sound_var_end(Sound_Vars *snd) {
    snd->time += snd->dt;
}

// Get a new persistent variable for this sample
static f32 *sound_var(Sound_Vars *sound) {
    if (sound->phase_ix >= array_count(sound->phase)) return 0;
    return sound->phase + sound->phase_ix++;
}

// Generate linear ramp from [0, 1) at a given frequency
static f32 sound_ramp(Sound_Vars *sound, f32 freq) {
    // Get a phase value
    f32 *phase = sound_var(sound);

    // Return current value (start at 0)
    f32 sample = *phase;

    // Increment phase
    *phase += sound->dt * freq;
    *phase -= (i32)*phase;
    return sample;
}

// Saw wave
static f32 sound_saw(Sound_Vars *sound, f32 freq) {
    return sound_ramp(sound, freq) * 2.0f - 1.0f;
}

static f32 sound_pulse(Sound_Vars *sound, f32 freq, f32 duty) {
    return sound_ramp(sound, freq) < duty * 0.5f;
}

// Generate a pure sine wave at a given frequency
// range = [-1, 1]
static f32 sound_sine(Sound_Vars *sound, f32 freq) {
    return f_sin2pi(sound_ramp(sound, freq));
}

static f32 sound_noise_white(Sound_Vars *sound) {
    return rand_f32_signed(&sound->rand);
}

static f32 sound_noise_freq(Sound_Vars *sound, f32 freq, f32 duty) {
    // Current sample
    f32 *value = sound_var(sound);

    // Timer until next sample
    f32 *timer = sound_var(sound);

    // Increment timer
    *timer += sound->dt * freq;

    if (*timer > duty) *value = 0.0f;

    // Timer triggered, get a new sample
    if (*timer > 1.0f) {
        *value = rand_f32_signed(&sound->rand);
        *timer -= (i32)*timer;
    }

    return *value;
}

typedef struct {
    f32 low_pass;
    f32 band_pass;
    f32 high_pass;
} Sound_Filter_Result;

// Filter the incoming samples at a given cutoff frequency.
static Sound_Filter_Result sound_filter(Sound_Vars *sound, f32 cutoff_freq, f32 sample) {
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

typedef enum {
    Sound_Sine,
    Sound_Pulse,
    Sound_Saw,
    Sound_Noise,
} Sound_Source;

// A single sound
typedef struct {
    u32 id;

    f32 base_volume;
    f32 base_freq;
    f32 base_duty;

    // Settings
    Sound_Source source;

    Sound_Envelope volume;
    Sound_Envelope duty;
    Sound_Envelope freq;

    Sound_Vars vars;
} Sound;

static Sound sound_new(void) {
    Sound snd = {};
    snd.id = id_next();
    snd.source = Sound_Sine;
    snd.base_freq = 440;
    snd.base_duty = 1.0;
    snd.base_volume = 1;

    snd.volume.sustain = 1;
    snd.freq.sustain = 100;
    snd.duty.sustain = 100;
    snd.vars.dt = 1.0f / AUDIO_SAMPLE_RATE;
    return snd;
}

static Sound_Result sound_sample(Sound *snd) {
    sound_var_begin(&snd->vars);
    f32 time = snd->vars.time;

    //
    Sound_Result envelope = sound_envelope(&snd->volume, time);
    if (envelope.done) return (Sound_Result){.done = 1};

    f32 volume = snd->base_volume * envelope.volume;
    f32 freq = snd->base_freq * sound_envelope(&snd->freq, time).volume;
    f32 duty = snd->base_duty * sound_envelope(&snd->duty, time).volume;

    f32 source = 0;
    if (snd->source == Sound_Sine) source += sound_sine(&snd->vars, freq);
    if (snd->source == Sound_Pulse) source += sound_pulse(&snd->vars, freq, duty);
    if (snd->source == Sound_Saw) source += sound_saw(&snd->vars, freq);
    if (snd->source == Sound_Noise) source += sound_noise_freq(&snd->vars, freq, duty);

    f32 sample = source * volume;

    sound_var_end(&snd->vars);
    return (Sound_Result){.volume = sample};
}
