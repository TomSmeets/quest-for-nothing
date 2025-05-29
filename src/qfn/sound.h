// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sound.h - Immediate mode sound synthesis
#pragma once
#include "math.h"
#include "rand.h"
#include "sound_effect.h"
#include "sound_env.h"
#include "sound_filter.h"
#include "sound_music.h"
#include "sound_osc.h"
#include "sound_var.h"
#include "types.h"

// https://www.eetimes.com/making-sounds-with-analogue-electronics-part-1-before-the-synthesizer/
// FM: https://www.youtube.com/watch?v=DD0UpZ5uGAo

// ============== Clipping =======

// ============== Filter ==============

static f32 sound_piano_key(Sound *sound, f32 freq, bool down) {
    f32 s0 = sound_adsr(sound, down, 8.0f, 1.0f, 0.5f, 1.0f) * sound_sine(sound, 4, 0);
    f32 s1 = sound_adsr(sound, down, 2.0f, 1.0f, 0.5f, 1.0f) * sound_sine(sound, freq, s0*.2);
    f32 s2 = sound_adsr(sound, down, 16.0f, 1.0f, 0.5f, 1.0f) * sound_sine(sound, freq, s1+s0*.0);
    f32 s3 = sound_adsr(sound, down, 100.0f, 40.0f, 0.0f, 40.0f) * sound_noise_white(sound);
    // out = sound_lowpass(sound, freq, out);

    // out += volume*sound_sine(sound, freq, s1*s1*.4);
    // out += volume*sound_sine(sound, freq, s1*s1*s1*.5);
    // out = volume*sound_saw(sound, freq, sound_sine(sound, freq, 0)*volume*.5);
    return s2 + s3;
}

typedef struct {
    bool down;
    f32 note;
} Poly_Voice;

typedef struct {
    u32 index;
    Poly_Voice voices[8];
} Poly;

static void poly_play(Poly *poly, f32 note) {
    for (u32 i = 0; i < array_count(poly->voices); ++i) poly->voices[i].down = false;

    Poly_Voice *voice = poly->voices + poly->index;
    voice->down = true;
    voice->note = note;
    poly->index++;
    poly->index %= array_count(poly->voices);
}

static f32 sound_music(Sound *sound) {
    f32 out = 0.0f;

    Poly *poly = sound_var(sound, Poly);
    Clock clk = sound_clock(sound, 1.0f / 2);
    if (clk.trigger) {
        u32 note = rand_u32(&sound->rand, 35-7 , 35+7);
        f32 freq = sound_scale(note);
        poly_play(poly, freq);
    }

    bool down = f_fract(clk.phase * 2) < 0.5;

    // Delay_Buffer *buf = sound_var(sound, Delay_Buffer);
    // out += 0.8 * delay_read(buf, 63.0f / SOUND_SAMPLE_RATE);

    // clk.phase < 0.25f;
    for (u32 i = 0; i < 8; ++i) {
        Poly_Voice *voice = poly->voices + i;
        out += 0.5 * sound_piano_key(sound, voice->note, voice->down && down);
    }
    out += 0.5 * sound_piano_key(sound, OCT_4 * NOTE_C, clk.phase < 0.5);
    out += 0.5 * sound_piano_key(sound, OCT_3 * NOTE_F, clk.phase > 0.5);
    // delay_write(buf, out);
    return out;
}
