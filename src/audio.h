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
    f32 dt;
    Random rand;

    u32 sound_count;
    Sound sounds[16];
} Audio;

static Audio *audio_new(Memory *mem) {
    return mem_struct(mem, Audio);
}

static void audio_play(Audio *audio, Sound new_sound) {
    assert(new_sound.id != 0, "Sound is invalid");

    for (u32 i = 0; i < array_count(audio->sounds); ++i) {
        Sound *sound = audio->sounds + i;

        // Find Empty slot
        if (sound->id) continue;

        // Insert sound
        new_sound.vars.rand = rand_fork(&audio->rand);
        *sound = new_sound;

        fmt_suu(OS_FMT, "Playing #", new_sound.id, " in slot ", i, "\n");
        return;
    }

    fmt_s(OS_FMT, "Could not play sound\n");
}

static void audio_stop(Audio *audio, u32 id) {
    // Swap remove sound
    for (u32 i = 0; i < array_count(audio->sounds); ++i) {
        Sound *snd = audio->sounds + i;
        if (snd->id == id) snd->id = 0;
    }
}
