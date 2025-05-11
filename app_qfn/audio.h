// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// audio.h - Immediate mode audio synthesis and digital filters
#pragma once
#include "math.h"
#include "mem.h"
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
    Sound *sound = &voice->sound;
    f32 volume = sound_ar(voice->time, 0.02, 0.5f, &voice->done);
    f32 sample = 0.0f;
    sample += sound_saw(sound, voice->freq * BLEND(0.8f, 4.0, volume), 0);
    return volume * sample;
}

static f32 voice_noise(Voice *voice) {
    Sound *sound = &voice->sound;
    f32 v1 = sound_ar(voice->time, 0.50, 1.0f, &voice->done);
    f32 v2 = sound_ar(voice->time, 0.0, 0.4f, 0);
    f32 sample = 0;
    sample += v1 * sound_noise_freq(sound, voice->freq * v1 * 1.0f, 0.2f) * 0.5f;
    sample += v2 * sound_noise_freq(sound, voice->freq * v2 * 4.0f, 0.2f) * 2.0f;
    return sample;
}

static f32 voice_note(Voice *voice) {
    Sound *sound = &voice->sound;
    f32 volume = sound_ar(voice->time, 0.08, 1.0, &voice->done);
    f32 sample = 0;
    f32 offset = sound_sine(sound, 4, 0);

    f32 phase = volume * sound_sine(sound, voice->freq, offset * 0.1);
    sample += volume * sound_sine(sound, voice->freq, phase * 0.2);
    // sample = f_clamp(sample*4, -1.0f, 1.0f);
    // sample = sound_filter(sound, voice->freq, sample).low_pass;
    // sample *= 4;
    return sample;
}

static f32 voice_sample(Voice *voice) {
    f32 ret = 0.0f;
    sound_begin(&voice->sound);
    if (voice->kind == 0) ret = voice_pew(voice);
    if (voice->kind == 1) ret = voice_noise(voice);
    if (voice->kind == 2) ret = voice_note(voice);
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
    Rand rand;

    u32 voice_count;
    Voice voices[64];
} Audio;

static Audio *audio_new(Memory *mem) {
    return mem_struct(mem, Audio);
}

static void audio_play(Audio *audio, Voice voice) {
    if (audio->voice_count == array_count(audio->voices)) return;
    audio->voices[audio->voice_count++] = voice;
}

static f32 audio_sample(Audio *audio) {
    f32 res = 0.0f;
    for (u32 i = 0; i < audio->voice_count;) {
        Voice *voice = audio->voices + i;
        res += voice_sample(voice);

        if (voice->done) {
            // Swap remove
            audio->voices[i] = audio->voices[--audio->voice_count];
        } else {
            // Go to next
            i++;
        }
    }
    return res;
}
