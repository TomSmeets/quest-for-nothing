// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sound_filter.h - Sound filters
#pragma once
#include "sound.h"

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
    f32 f = SOUND_DT / (rc + SOUND_DT);

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

static f32 sound_lowpass(Sound *sound, f32 cutoff_freq, f32 sample) {
    return sound_filter(sound, cutoff_freq, sample).low_pass;
}
