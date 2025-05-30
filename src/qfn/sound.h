// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sound.h - Immediate mode sound synthesis
#pragma once
#include "math.h"
#include "vec.h"
#include "fmt.h"
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

static f32 music_note(Sound *sound, bool down, f32 freq) {
    float a = 0.5f;
    float d = 0.5f;
    float s = 0.5f;

    f32 volume = sound_adsr(sound, down, a, d, s);
    f32 out = 0.0f;
    if(1) {
        out += volume * sound_saw(sound, freq, 0);
        out += volume * sound_saw(sound, freq * 1.001, 0);
    } else {
        out += volume * sound_sine(sound, freq, sound_sine(sound, freq, 0)*0.4);
        out += volume * sound_sine(sound, freq*1.001, sound_sine(sound, freq*1.001, 0)*0.4);
    }
    out = sound_lowpass(sound, 100.0f, out);
    return out;
}

static f32 music_base(Sound *sound, u32 beat) {
    float base_c = OCT_3 * NOTE_C;
    float base_f = OCT_2 * NOTE_F;
    u32 note = (beat / 8) % 2;

    float out = 0.0f;
    out += music_note(sound, note == 0, base_c);
    out += music_note(sound, note == 1, base_f);
    return out;
}

static f32 music_melody(Sound *sound, u32 beat) {
    // Every whole note
    u32 note = beat / 4;

    u32 voice_ix = note % 2;
    f32 *voice_freq = sound_vars(sound, f32, 2);
    if(sound_changed(sound, note)) {
        fmt_su(G->fmt, "V: ", voice_ix, "\n");
        f32 freq = sound_scale(rand_u32(&sound->rand, 7 * 5, 7 * 6 + 1));
        voice_freq[voice_ix] = freq;
    }

    f32 out = 0.0f;
    out += music_note(sound, voice_ix == 0, voice_freq[0]);
    out += music_note(sound, voice_ix == 1, voice_freq[1]);
    return out;
}

static v2 sound_music(Sound *sound) {
    v2 out = { 0, 0 };

    Clock clk = sound_clock(sound, 1.0f, 32);
    if (clk.trigger) fmt_su(G->fmt, "IX: ", clk.index, "\n");

    {
        f32 music = 0.0f;
        music += music_base(sound, clk.index);
        music += music_melody(sound, clk.index)*.5;

        music += 0.1 * (clk.index / 8 == 0) * ((f32)(clk.index % 8) / 8) * 0.5 * clk.phase * sound_pulse(sound, NOTE_C * clk.phase, 0, 0.5f);
        music = sound_lowpass(sound, 100.0f, music);
        out.x += music;
        out.y += music;
    }
    out = sound_reverb2(sound, out)*2;
    // out *= 0.5f;
    // out2.x = out;
    // out2.y = out;
    return out;
}
