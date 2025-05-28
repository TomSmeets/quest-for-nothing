// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sound.h - Immediate mode sound synthesis
#pragma once
#include "math.h"
#include "rand.h"
#include "types.h"

#define SOUND_SAMPLE_RATE 48000
#define SOUND_DT (1.0f / SOUND_SAMPLE_RATE)

// https://www.eetimes.com/making-sounds-with-analogue-electronics-part-1-before-the-synthesizer/
// FM: https://www.youtube.com/watch?v=DD0UpZ5uGAo

typedef struct {
    // For noise
    Rand rand;

    // List of 'phase' values for sine waves.
    u32 phase_ix;
    f32 phase[32];
} Sound;


// Attack-Release envelope
//      _
//     /.\
//    / . \
//   /  .  \
// _/   .   \_____
//  . A . R .
static f32 sound_ar(f32 time, f32 t_attack, f32 t_release, bool *done) {
    f32 value = 0.0f;
    value += f_step_duration(time, t_attack);
    time -= t_attack;
    value -= f_step_duration(time, t_release);
    time -= t_release;
    // Signal done
    if (done && time >= 0) *done = true;
    return value;
}

// Attack-Decay-Sustain-Release envelope.
//      _
//     /.\
//    / . \_______
//   /  . .     . \
// _/   . .     .  \___
//  . A .D.  S  . R .
static f32 sound_adsr(f32 time, f32 t_attack, f32 t_decay, f32 t_sustain, f32 t_release, bool *done) {
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

// Start producing a new sound sample
static void sound_begin(Sound *snd) {
    snd->phase_ix = 0;
}

// Get a new persistent variable for this sample
static f32 *sound_var(Sound *sound) {
    assert(sound->phase_ix < array_count(sound->phase), "Out of sound memory, Increase the maximum number of phases");
    return sound->phase + sound->phase_ix++;
}

// Generate linear ramp from [0, 1) at a given frequency and phase offset
static f32 sound_ramp(Sound *sound, f32 freq, f32 offset) {
    // Get a phase value
    f32 *phase = sound_var(sound);

    // Compute current phase with offset
    f32 out = f_fract(*phase + offset);

    // Compute next variable
    *phase = f_fract(*phase + SOUND_DT * freq);

    return out;
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

static f32 sound_triangle(Sound *sound, f32 freq, f32 offset) {
    f32 v = sound_ramp(sound, freq, offset);
    return f_min(v * 4 - 1, 3 - v * 4);
}

// Static white noise
static f32 sound_noise_white(Sound *sound) {
    return rand_f32(&sound->rand, -1, 1);
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
    *phase += SOUND_DT * freq;

    // Reached next cycle
    if (*phase >= 1.0) {
        *value = rand_f32(&sound->rand, -1, 1);
        *phase -= (i32)*phase;
    }

    return ret;
}

// Midi note to freq
static f32 sound_note(u8 note) {
    f32 octave_list[] = {
        1.0f / 32.0f, // -1
        1.0f / 16.0f, // 0
        1.0f / 8.0f,  // 1
        1.0f / 4.0f,  // 2
        1.0f / 2.0f,  // 3
        1.0f,         // 4
        2.0f,         // 5
        4.0f,         // 6
        8.0f,         // 7
        16.0f,        // 8
        32.0f,        // 9
    };

    f32 note_list[] = {
        261.62556530059860, // C
        277.18263097687210, // C#
        293.66476791740760, // D
        311.12698372208090, // D#
        329.62755691286990, // E
        349.22823143300390, // F
        369.99442271163440, // F#
        391.99543598174927, // G
        415.30469757994510, // G#
        440.00000000000000, // A
        466.16376151808990, // A#
        493.88330125612410, // B
    };

    u8 note_ix = note % 12;
    u8 octave_ix = note / 12;
    return octave_list[octave_ix] * note_list[note_ix];
}
