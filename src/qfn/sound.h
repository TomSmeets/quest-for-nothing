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

// static f32 sound_piano_key(Sound *sound, f32 freq, bool down) {
// f32 s0 = sound_adsr(sound, down, 8.0f, 1.0f, 0.5f, 1.0f) * sound_sine(sound, 4, 0);
// f32 s1 = sound_adsr(sound, down, 2.0f, 1.0f, 0.5f, 1.0f) * sound_sine(sound, freq, s0*.2);
// f32 s2 = sound_adsr(sound, down, 16.0f, 1.0f, 0.5f, 1.0f) * sound_sine(sound, freq, s1+s0*.0);
// f32 s3 = sound_adsr(sound, down, 100.0f, 40.0f, 0.0f, 40.0f) * sound_noise_white(sound);
// out = sound_lowpass(sound, freq, out);

// out += volume*sound_sine(sound, freq, s1*s1*.4);
// out += volume*sound_sine(sound, freq, s1*s1*s1*.5);
// out = volume*sound_saw(sound, freq, sound_sine(sound, freq, 0)*volume*.5);
// return s2 + s3;
// }

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

    Clock clk = sound_clock(sound, 1.0f, 4);
    f32 *note0 = sound_var(sound, f32);
    f32 *note1 = sound_var(sound, f32);
    f32 *note2 = sound_var(sound, f32);
    f32 *note3 = sound_var(sound, f32);
    if (clk.trigger) {
        u32 note_min = 35;
        u32 note_max = 40;
        if (clk.index == 0) *note0 = sound_scale(rand_u32(&sound->rand, note_min, note_max));
        if (clk.index == 1) *note1 = sound_scale(rand_u32(&sound->rand, note_min, note_max));
        if (clk.index == 2) *note2 = sound_scale(rand_u32(&sound->rand, note_min, note_max));
        if (clk.index == 3) *note3 = sound_scale(rand_u32(&sound->rand, note_min, note_max));
    }
    out += 0.5 * sound_adsr(sound, !clk.trigger, 80, 40, 0) * sound_noise_white(sound);

    f32 offset = sound_sine(sound, 4, 0);
    out += 0.5 * sound_adsr(sound, clk.index == 0, 8, 1, 0.8) * sound_sine(sound, *note0 * (1 + offset * 0.01), sound_sine(sound, *note0, 0) * .5);
    out += 0.5 * sound_adsr(sound, clk.index == 1, 8, 1, 0.8) * sound_sine(sound, *note1 * (1 + offset * 0.01), sound_sine(sound, *note1, 0) * .5);
    out += 0.5 * sound_adsr(sound, clk.index == 2, 8, 1, 0.8) * sound_sine(sound, *note2 * (1 + offset * 0.01), sound_sine(sound, *note2, 0) * .5);
    out += 0.5 * sound_adsr(sound, clk.index == 3, 8, 1, 0.8) * sound_sine(sound, *note3 * (1 + offset * 0.01), sound_sine(sound, *note3, 0) * .5);

    out += 0.5 * sound_adsr(sound, clk.index / 2 == 0, 8, 1, 0.8) * sound_sine(sound, OCT_4 * NOTE_C, 0);
    out += 0.5 * sound_adsr(sound, clk.index / 2 == 1, 8, 1, 0.8) * sound_sine(sound, OCT_3 * NOTE_F, 0);
    return out;
}
