// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sound_filter.h - Sound filters
#pragma once
#include "gfx/sound_var.h"
#include "lib/fmt.h"
#include "lib/vec.h"

typedef struct {
    f32 low_pass;
    f32 band_pass;
    f32 high_pass;
} Sound_Filter_Result;

// Filter the incoming samples at a given cutoff frequency.
static Sound_Filter_Result sound_filter(Sound *sound, f32 cutoff_freq, f32 sample) {
    f32 *var0 = sound_var(sound, f32);
    f32 *var1 = sound_var(sound, f32);

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
    f32 *value = sound_var(sound, f32);
    f32 a = 1.0f - f_exp(-SOUND_DT * PI2 * cutoff_freq);
    f32 ret = *value;
    *value += (sample - ret) * a;
    return ret;
}

// https://github.com/sinshu/freeverb/blob/main/Components/comb.hpp
static f32 sound_comb(Sound *sound, u32 size, f32 damp, f32 feedback, f32 input) {
    f32 *buf = sound_vars(sound, f32, size);
    f32 *filter = sound_var(sound, f32);
    u32 *ix = sound_var(sound, u32);
    if (*ix >= size) *ix = 0;

    f32 output = buf[*ix];

    // Low pass filter
    *filter += (output - *filter) * (1.0f - damp);

    // Delay line + Feedback
    buf[*ix] = input + feedback * (*filter);
    (*ix)++;
    return output;
}

// https://github.com/sinshu/freeverb/blob/main/Components/allpass.hpp
static f32 sound_allpass(Sound *sound, u32 size, f32 feedback, f32 input) {
    f32 *buf = sound_vars(sound, f32, size);
    u32 *ix = sound_var(sound, u32);
    if (*ix >= size) *ix = 0;

    f32 output = buf[*ix];
    buf[*ix] = input + feedback * output;
    (*ix)++;
    return output - input;
}

static f32 sound_freeverb(Sound *sound, u32 spread, f32 feedback, f32 damp, f32 input) {
    // Rescale factor for 44.1 KHz to 48 KHz
    f32 a = (f32)SOUND_SAMPLE_RATE / 44100.0f;

    f32 out = 0.0f;
    out += sound_comb(sound, (1116 + spread) * a, damp, feedback, input);
    out += sound_comb(sound, (1188 + spread) * a, damp, feedback, input);
    out += sound_comb(sound, (1277 + spread) * a, damp, feedback, input);
    out += sound_comb(sound, (1356 + spread) * a, damp, feedback, input);
    out += sound_comb(sound, (1422 + spread) * a, damp, feedback, input);
    out += sound_comb(sound, (1491 + spread) * a, damp, feedback, input);
    out += sound_comb(sound, (1557 + spread) * a, damp, feedback, input);
    out += sound_comb(sound, (1617 + spread) * a, damp, feedback, input);

    out = sound_allpass(sound, (556 + spread) * a, 0.5, out);
    out = sound_allpass(sound, (441 + spread) * a, 0.5, out);
    out = sound_allpass(sound, (341 + spread) * a, 0.5, out);
    out = sound_allpass(sound, (225 + spread) * a, 0.5, out);
    return out;
}

typedef struct {
    // Room size
    f32 room;

    // Sound Damping
    f32 damp;

    // How much of the reverb sound to mix in
    f32 wet;

    // How much of the original sound to mix in
    f32 dry;
} Freeverb_Config;

// Full freeverb implementation
static v2 sound_freeverb2(Sound *sound, Freeverb_Config cfg, v2 input) {
    u32 stero_spread = 23;
    f32 width = 1.0f;
    f32 gain = 0.015f;

    // Tuned params
    f32 room = cfg.room * 0.28f + 0.7f;
    f32 damp = cfg.damp * 0.40f;
    f32 wet = cfg.wet * 3.0f;
    f32 dry = cfg.dry * 2.0f;

    f32 wet_1 = wet * 0.5 * (width + 1);
    f32 wet_2 = wet * 0.5 * (1 - width);

    f32 real_input = (input.x + input.y) * gain;
    f32 l = sound_freeverb(sound, 0, room, damp, real_input);
    f32 r = sound_freeverb(sound, stero_spread, room, damp, real_input);

    f32 out_l = l * wet_1 + r * wet_2 + input.x * dry;
    f32 out_r = r * wet_1 + l * wet_2 + input.y * dry;
    return (v2){out_l, out_r};
}

__attribute__((aligned(16))) static const u8 SOUND_REVERB_IR[] = {
#embed "gfx/sound_ir.f32"
};

static v2 sound_reverb3(Sound *sound, v2 sample) {
    const v2 *buffer = (v2 *)SOUND_REVERB_IR;
    const u32 count = sizeof(SOUND_REVERB_IR) / sizeof(v2) / 8;

    v2 *samples = sound_vars(sound, v2, count);
    u32 *ix = sound_var(sound, u32);
    if (*ix >= count) *ix = 0;

    v2 out = 0;
    for (u32 i = 0; i < count; i+=4) {
        u32 j = *ix + count - i;
        if(j >= count) j -= count;
        out += samples[j] * buffer[i];
    }

    // Write sample
    samples[*ix] = sample;
    (*ix)++;

    return out;
}

static f32 sound_delay(Sound *sound, f32 sample, f32 time, f32 max) {
    if(time > max) time = max;
    if(time < 0) time = 0;

    u32 count = max * SOUND_SAMPLE_RATE + 1;
    u32 offset = time * SOUND_SAMPLE_RATE;
    if(offset >= count) offset = count - 1;

    f32 *samples = sound_vars(sound, f32, count);

    u32 *ix = sound_var(sound, u32);
    if(*ix >= count) *ix = 0;
    samples[*ix] = sample;
    f32 out = samples[(*ix + count - offset) % count];

    (*ix)++;
    return out;
}

static v2 sound_pan(Sound *sound, f32 sample, v3 dir) {
    f32 distance = v3_length(dir);
    if(distance > 0) dir /= distance;

    f32 scale = 0.6f / 1000 * 1;

    f32 ang_right = f_max(-dir.x, 0);
    f32 ang_left  = f_max(dir.x, 0);

    // x=left, y=right
    v2 out = 0;
    out.x = sound_delay(sound, sample, ang_right * scale, scale);
    out.y = sound_delay(sound, sample, ang_left  * scale, scale);

    out.x += (sound_filter(sound, 2000, out.x).low_pass - out.x) * (ang_right - (dir.z + 1) / 2 * 0.5);
    out.y += (sound_filter(sound, 2000, out.y).low_pass - out.y) * (ang_left - (dir.z + 1) / 2 * 0.5);

    f32 attenuation = (distance);
    if(attenuation > 1) out /= attenuation;
    return out;
}
