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
    bool done;
    f32 volume;
} Sound_Result;

// Sound Envelope
typedef struct {
    // Paramters
    f32 volume;
    f32 freq;
    f32 attack_time;
    f32 release_time;

    // Runtime info
    f32 phase;
} Sound_Source;

typedef struct {
    // Paramters
    f32 freq;

    // Sources
    Sound_Source src_a;
    Sound_Source src_b;
    Sound_Source src_c;
    Sound_Source src_d;

    // Runtime
    f32 dt;
    f32 time;
    bool playing;
    f32 filter0;
    f32 filter1;
    f32 feedback_value;
} Sound;

// Very simple envelope
static f32 sound_envelope(f32 time, f32 attack_time, f32 release_time) {
    f32 v_attack = f_step_duration(time, attack_time);
    f32 v_release = 1 - f_step_duration(time - attack_time, release_time);
    return f_min(v_attack, v_release);
}

// Increment phase by a given amount.
static f32 sound_phase(f32 *phase, f32 dt) {
    f32 value = *phase;
    *phase += dt;
    *phase -= (i32)*phase;
    return value;
}

static f32 sound_source(Sound *sound, Sound_Source *source, f32 input) {
    f32 envelope = sound_envelope(sound->time, source->attack_time, source->release_time);
    f32 phase = sound_phase(&source->phase, sound->dt * sound->freq * source->freq * (1 + input));
    return f_sin2pi(phase) * envelope * source->volume;
}

static void sound_reset(Sound *sound) {
    sound->dt = 1.0f / AUDIO_SAMPLE_RATE;
    // sound->time = 0;
    sound->src_a.phase = 0;
    sound->src_b.phase = 0;
    sound->src_c.phase = 0;
    sound->src_d.phase = 0;
    sound->filter0 = 0;
    sound->filter1 = 0;
    sound->feedback_value = 0;
    sound->playing = true;
}

typedef struct {
    f32 low_pass;
    f32 band_pass;
    f32 high_pass;
} Sound_Filter_Result;

// Filter the incoming samples at a given cutoff frequency.
static Sound_Filter_Result sound_filter(Sound *sound, f32 cutoff_freq, f32 sample) {
    f32 rc = 1.0 / (cutoff_freq * PI2);
    f32 f = sound->dt / (rc + sound->dt);

    // f and fb calculation
    f32 q = 0.9;
    f32 fb = q + q / (1.0 - f);

    // loop

    // High Pass Filter
    f32 hp = sample - sound->filter0;

    // Band Pass Filter
    f32 bp = sound->filter0 - sound->filter1;

    sound->filter0 += f * (hp + fb * bp);
    sound->filter1 += f * (sound->filter0 - sound->filter1);

    // Low Pass Filter
    f32 lp = sound->filter1;

    return (Sound_Filter_Result){lp, bp, hp};
}

static f32 sound_sample(Sound *sound) {
    if (!sound->playing) return 0;

    f32 v_d = sound_source(sound, &sound->src_d, 0);
    f32 v_c = sound_source(sound, &sound->src_c, v_d);
    f32 v_b = sound_source(sound, &sound->src_b, v_c);
    f32 v_a = sound_source(sound, &sound->src_a, v_b);
    sound->time += sound->dt;

    if (sound->time > sound->src_a.attack_time + sound->src_a.release_time) {
        sound->playing = false;
    }

    // v_a = sound_filter(sound, sound->freq*2, v_a).low_pass;
    return v_a;
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
