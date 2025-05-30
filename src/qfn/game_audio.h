// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// game_audio.h - Audio effects and Music
#pragma once
#include "engine.h"
#include "music.h"

// Play Jump sound
static void game_audio_jump(Engine *eng) {
    gfx_audio_lock(eng->gfx);
    eng->audio->play_jump = 1;
    gfx_audio_unlock(eng->gfx);
}

// Play Gun shooting sound
static void game_audio_shoot(Engine *eng) {
    gfx_audio_lock(eng->gfx);
    eng->audio->play_shoot = 1;
    gfx_audio_unlock(eng->gfx);
}
