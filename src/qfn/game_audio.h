// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// game_audio.h - Audio effects and Music
#pragma once
#include "rand.h"
#include "sound.h"
#include "sound_filter.h"
#include "types.h"

// Sound System

// References:
//   How to disscet a Melody: https://www.youtube.com/watch?v=PZ5POrP-NMw
//   - Melodic Embellishments + Melodic Reduction
//   Learn every chord and chord symbol: https://www.youtube.com/watch?v=CyNiY1jzOuQ
//   - Chords vs Melody
// Inspiration
//   - https://www.youtube.com/watch?v=J_FCCvNzbiY
typedef struct {
    Sound snd;
    bool play_shoot;
    bool play_jump;
} Audio;

// Play Jump sound
static void audio_jump(Audio *audio) {
    // TODO: Mutex
    audio->play_jump = 1;
}

// Play Gun shooting sound
static void audio_shoot(Audio *audio) {
    // TODO: Mutex
    audio->play_shoot = 1;
}

static v2 audio_sample(Audio *audio) {
    Sound *sound = &audio->snd;

    v2 out = {0, 0};
    Clock clk = sound_clock(sound, 1.0f, 32);

    sound_begin(sound);
    f32 base = 0.2f * music_base(sound, clk.index);
    f32 melo = 0.3f * music_melody(sound, clk.index);
    f32 chance = 1.0f / 128.0f;
    f32 volume = 0.04f;

    bool play_noise0 = clk.trigger && rand_choice(G->rand, chance);
    bool play_noise1 = clk.trigger && rand_choice(G->rand, chance);
    bool play_noise2 = clk.trigger && rand_choice(G->rand, chance);
    bool play_noise3 = clk.trigger && rand_choice(G->rand, chance);
    bool play_noise4 = clk.trigger && rand_choice(G->rand, chance);

    melo += volume * sound_adsr(sound, play_noise0, 400, 1.0, 0) * sound_noise_white(sound);
    melo += volume * sound_adsr(sound, play_noise1, 2, 1.0, 0) * clk.phase * sound_pulse(sound, NOTE_C * clk.phase, 0, 0.5f);
    melo += volume * sound_adsr(sound, play_noise2, 0.5, 0.5, 0) * sound_sine(sound, NOTE_C * (1 + sound_sine(sound, 4, 0) * .5f), 0);
    melo += volume * sound_adsr(sound, play_noise3, 1, 0.5, 0) * sound_sine(sound, NOTE_F * (1 + sound_sine(sound, 2, 0) * .5f), 0);
    melo += volume * sound_adsr(sound, play_noise4, 4, 1, 0) * sound_sine(sound, NOTE_C * (1 + sound_sine(sound, 8, 0) * .5f), 0);

    f32 jump_vol = sound_adsr(sound, audio->play_jump, 400, 4.0, 0);
    melo += 0.1 * jump_vol * sound_saw(sound, NOTE_C * (1 + 0.2 * sound_sine(sound, 8, 0)), 0);
    melo += 1.0 * sound_lowpass(
                      sound, NOTE_C,
                      sound_adsr(sound, audio->play_shoot, 8000, 16.0, 0) * (sound_noise_white(sound) + sound_noise_freq(sound, NOTE_C / 4, 1.0f))
                  );
    audio->play_jump = 0;
    audio->play_shoot = 0;

    melo += base;
    // melo *= 0.5f;
    out.x += melo;
    out.y += melo;
    out *= 0.25f;
    out += sound_reverb2(sound, out);
    out *= 1.0f;
    // out2.x = out;
    // out2.y = out;
    return out;
}
