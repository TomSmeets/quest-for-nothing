// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// game_audio.h - Audio effects and Music
#pragma once
#include "engine.h"
#include "music.h"

// Play Jump sound
static void game_audio_jump(Engine *eng) {
    Voice snd = {};
    snd.freq = music_note_to_freq(7 * 3 + rand_u32(&eng->rng, 0, 7));
    snd.kind = 0;

    gfx_audio_lock(eng->gfx);
    audio_play(eng->audio, snd);
    gfx_audio_unlock(eng->gfx);
}

// Play Gun shooting sound
static void game_audio_shoot(Engine *eng) {
    Voice snd = {};
    snd.freq = music_note_to_freq(7 * 2 + rand_u32(&eng->rng, 0, 5));
    snd.kind = 1;

    gfx_audio_lock(eng->gfx);
    audio_play(eng->audio, snd);
    gfx_audio_unlock(eng->gfx);
}
