// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// game_audio.h - Audio effects and Music
#pragma once
#include "engine.h"

// Play Jump sound
static void game_audio_jump(Engine *eng) {
    Sound snd = {};
    snd.freq = sound_note_to_freq(-12 * 3.5 + rand_u32_range(&eng->rng, 0, 6));
    snd.src_a.freq = 1;
    snd.src_a.volume = 1;
    snd.src_a.attack_time = 0.0;
    snd.src_a.release_time = 1.0;

    snd.src_b.freq = 10;
    snd.src_b.volume = 10;
    snd.src_b.attack_time = 0.0;
    snd.src_b.release_time = 2.0;

    snd.src_c.freq = 1;
    snd.src_c.volume = .5;
    snd.src_c.attack_time = 0.1;
    snd.src_c.release_time = 2.0;
    audio_play(eng->audio, snd);
}

// Play Gun shooting sound
static void game_audio_shoot(Engine *eng) {
    Sound snd = {};
    snd.freq = sound_note_to_freq(-12 * 3 + rand_u32_range(&eng->rng, 0, 4));
    snd.src_a.freq = 1;
    snd.src_a.volume = 1;
    snd.src_a.attack_time = 0.0;
    snd.src_a.release_time = 1.5;

    snd.src_b = snd.src_a;
    snd.src_b.release_time *= 2.0;
    snd.src_b.freq = 400;
    snd.src_b.volume = 18;

    snd.src_c = snd.src_b;
    snd.src_d = snd.src_b;
    audio_play(eng->audio, snd);
}
