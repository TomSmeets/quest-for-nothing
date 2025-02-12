// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// audio.h - Immediate mode audio synthesis and digital filters
#pragma once
#include "math.h"
#include "rand.h"
#include "sound.h"
#include "types.h"


typedef struct {
    float freq;
    float time;
    bool done;
    u32 kind;
    Sound sound;
} Voice;

static f32 voice_pew(Voice *voice) {
 Sound *sound =& voice->sound;
    f32 volume = sound_ar(voice->time, 0.01, 1.0f, &voice->done);
    f32 sample = 0.0f;
    sample += sound_saw(sound, voice->freq * BLEND(0.8f, 1.0, volume), 0);
    return volume * sample;
}

static f32 voice_noise(Voice *voice) {
 Sound *sound =& voice->sound;
    f32 volume = sound_ar(voice->time, 0.01, 1.0f, &voice->done);
    f32 sample = sound_noise_freq(sound, voice->freq, 0);
    return volume*sample;
}

static f32 voice_sample(Voice *voice) {
    f32 ret = 0.0f;
    if (voice->kind == 0) ret = voice_pew(voice);
    if (voice->kind == 1) ret = voice_noise(voice);
    voice->time += SOUND_DT;
    return ret;
}

// Sound System

// References:
//   How to disscet a Melody: https://www.youtube.com/watch?v=PZ5POrP-NMw
//   - Melodic Embellishments + Melodic Reduction
//   Learn every chord and chord symbol: https://www.youtube.com/watch?v=CyNiY1jzOuQ
//   - Chords vs Melody
// Inspiration
//   - https://www.youtube.com/watch?v=J_FCCvNzbiY
typedef struct {
    f32 dt;
    Random rand;

    u32 voice_count;
    Voice voices[64];
} Audio;

static Audio *audio_new(Memory *mem) {
    return mem_struct(mem, Audio);
}

static void audio_play(Audio *audio, Voice voice) {
    if(audio->voice_count == array_count(audio->voices)) return;
    audio->voices[audio->voice_count++] = voice;
}

static f32 audio_sample(Audio *audio) {
    f32 res = 0.0f;
    for(u32 i = 0; i < audio->voice_count;) {
        Voice *voice = audio->voices + i;
        res += voice_sample(voice);

        if(voice->done) {
            // Swap remove
            audio->voices[i] = audio->voices[--audio->voice_count];
        } else {
            // Go to next
            i++;
        }
    }
    return res;
}
