// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// audio.h - Immediate mode audio synthesis and digital filters
#pragma once
#include "math.h"
#include "mem.h"
#include "rand.h"
#include "sound.h"
#include "sound_filter.h"
#include "types.h"

typedef struct {
    float freq;
    float time;
    float duration;
    float velocity;
    bool done;
    u32 kind;
    Sound sound;
} Voice;

#if 0
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
    f32 volume = sound_ar(voice->time, 0.02 * 50.0f / voice->velocity, voice->duration, &voice->done);
    f32 sample = 0;

    f32 offset = (1.0f - volume) * sound_sine(sound, 4, 0);
    f32 phase = volume * sound_sine(sound, voice->freq, 0);
    sample += volume * sound_sine(sound, voice->freq, phase * .25 + offset * .25);
    sample *= 0.4;
    // sample = sound_filter(sound, voice->freq, sample).low_pass;
    // sample *= 4;
    return sample;
}


static f32 sound_pling(Sound *sound, u8 note, f32 time) {
    float volume = sound_ar(time, 0.1, 4.0f, 0);
    float sample = 0;
    float freq = sound_note(note);

    // f32 phase2 = sound_ar(time, 1.0, 1.0f, 0) * sound_sine(sound, 4, 0);
    float phase = sound_ar(time, 0.05, 4.0f, 0) * sound_sine(sound, freq, 0);
    sample += sound_ar(time, 0.01, 4.0f, 0) * sound_sine(sound, freq, phase);
    // sample = f_clamp(sample * (1 + 1*sound_ar(time, 1.0, 4.0f, 0)), -1, 1);
    return sample;
}

static f32 voice_music(Voice *voice) {
    Sound *sound = &voice->sound;

    // C4 -> 5*12 
    float sample = 0;
    // float t1 = sound_time(sound, 0, 1, 4);
    // float t2 = sound_time(sound, 2, 1, 4);
    // sample += sound_pling(sound, 4*12 + 0, t1);
    // sample += sound_pling(sound, 3*12 + 5, t2);

    // f32 s0 =  f_sin2pi(sound_time(sound, 0,  8.0f, 1.0f));
    // f32 s3 =  f_sin2pi(sound_time(sound, 0,  1, 1.0f));
    // f32 s2 =  f_sin2pi(sound_time(sound, s3, sound_note(5*12), 1.0f));
    // f32 s1 =  f_sin2pi(sound_time(sound, s2*2, 80*4, 1.0f));
 // sample += f_sin2pi(sound_time(sound, 0, 440, 1.0f));
    // s1 = f_clamp(s1*8, -1, 1);
    // s2 = f_clamp(s2*4, -1, 1);
    // sample += s2;
    // f32 s2 =  sound_sine2(t*440);
    // sample += sound_noise(t*440/SOUND_SAMPLE_RATE);
    // sample += sound_ar(t2, 0.5, 3,0)*s2;
    // sample += sound_pling(sound, 65);
    sample = sound_music(sound);
    sample *= 0.4;
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
    if (voice->kind == 3) ret = voice_music(voice);
    voice->time += SOUND_DT;
    return ret;
}
#endif

static f32 voice_sample(Voice *voice) {
    sound_begin(&voice->sound);
    f32 ret = 0.0f;
    ret += sound_music(&voice->sound);
    ret *= 0.4f;
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
