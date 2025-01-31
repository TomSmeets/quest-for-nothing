// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// music.h - Music generation
#pragma once

#include "types.h"


// 4/4 timing
// 
// | 0123 | 4567 | 89ab | cdef |

typedef struct {
    f32 beat_per_second;
    f32 beat_time;
    u32 beat;
    bool beat_changed;
} Music;

static void music_init(Music *music) {
    music->beat_per_second = 120.0f / 60.0f;

    // First beat
    music->beat_changed = true;
}

static void music_play(Music *music, f32 dt) {
    u32 beat = music->beat;
    if(music->beat_changed) {
        audio_play(audio, snd)
    }

    music->beat_time += dt*music->beat_per_second;
    if(music->beat_time >= 1) {
        music->beat_time -= (i32) music->beat_time;
        music->beat++;
        music->beat_changed = true;
    }

    if(music->beat == 16) {
        music->beat = 0;
    }
}
