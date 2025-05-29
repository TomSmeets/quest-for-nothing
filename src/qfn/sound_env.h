
// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sound_env.h - Sound Envelopes
#pragma once
#include "sound_var.h"

static f32 sound_adsr(Sound *sound, bool down, f32 attack, f32 decay, f32 sustain, f32 release) {
    // State | Down |
    //     _ |    0 | T=0.0     Release
    //     0 |    1 | T=1.0     (Attack)
    //     1 |    1 | T=Sustain (Decay/Sustain)
    f32 *value = sound_var(sound, f32);
    bool *attack_done = sound_var(sound, bool);

    // Exponential target and speed
    f32 speed;
    f32 target;
    if(!down) {
        // Release
        *attack_done  = 0;
        target = 0.0f;
        speed  = release;
    } else if(*attack_done) {
        // Sustain
        target = sustain;
        speed  = decay;
    } else {
        // Attack (target a bit higher so that 1.0 is actually reached)
        target = 1.5f;
        speed  = attack;
        if (*value >= 1.0f) *attack_done = 1;
    }

    // Actual value should be this:
    float a = 1 - f_exp(-speed * SOUND_DT);
    *value += a * (target - *value);
    return *value;
}
