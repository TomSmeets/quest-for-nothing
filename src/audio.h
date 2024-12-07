// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// audio.h - Immediate mode audio synthesis and digital filters
#pragma once
#include "id.h"
#include "math.h"
#include "rand.h"
#include "sound.h"
#include "std.h"
#include "types.h"

// Sound System
typedef struct {
    // RNG to seed sound rngs
    f32 dt;
    Random rand;

    u32 sound_count;
    Sound sounds[16];
} Audio;


static Audio *audio_new(Memory *mem) {
    return mem_struct(mem, Audio);
}

static u32 audio_play(Audio *audio, u32 kind, f32 duration, f32 pitch) {
    for (u32 i = 0; i < array_count(audio->sounds); ++i) {
        Sound *snd = audio->sounds + i;
        if (snd->id) continue;
        std_memzero(snd, sizeof(*snd));

        u32 id = id_next();
        snd->rand = rand_fork(&audio->rand);
        snd->id = id;
        snd->dt = audio->dt;
        snd->kind = kind;
        snd->time = duration;
        snd->pitch = pitch;
        fmt_su(OS_FMT, "Playing #", id, "\n");
        return id;
    }
    return 0;
}

static void audio_stop(Audio *audio, u32 id) {
    // Swap remove sound
    for (u32 i = 0; i < array_count(audio->sounds); ++i) {
        Sound *snd = audio->sounds + i;
        if (snd->id == id) snd->id = 0;
    }
}
