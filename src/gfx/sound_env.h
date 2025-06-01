
// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sound_env.h - Sound Envelopes
#pragma once
#include "gfx/sound_var.h"

static f32 sound_adsr(Sound *sound, bool down, f32 attack, f32 decay, f32 sustain) {
    // State | Down |
    //     0 | Release
    //     1 | Attack
    //     2 | Sustain
    //
    f32 *value = sound_var(sound, f32);
    u32 *state = sound_var(sound, u32);
    if (*state > 2) *state = 0;

    // Release -> Attack
    if (*state == 0 && down) *state = 1;

    // Attack -> Sustain
    if (*state == 1 && *value >= 1.0f) *state = 2;

    // Sustain -> Release
    if (*state == 2 && !down) *state = 0;

    // Exponential target and speed
    f32 speed = 0.0f;
    f32 target = 0.0f;
    if (*state == 0) {
        // Release
        speed = decay;
        target = 0.0f;
    } else if (*state == 1) {
        // Attack
        speed = attack;
        target = 1.5f;
    } else if (*state == 2) {
        // Release
        speed = decay;
        target = sustain;
    }

    // Actual value should be this:
    float a = 1 - f_exp(-speed * SOUND_DT);
    *value += a * (target - *value);
    if (*value > 1.0f) *value = 1.0f;
    return *value;
}
