// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// game_audio.h - Audio effects and Music
#pragma once
#include "engine.h"

// Play Jump sound
static void game_audio_jump(Engine *eng) {
    Sound *snd = audio_play(eng->audio);
}

// Play Gun shooting sound
static void game_audio_shoot(Engine *eng) {
    Sound *snd = audio_play(eng->audio);
}
