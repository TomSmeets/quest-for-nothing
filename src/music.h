// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// music.h - Music generation
#pragma once
#include "types.h"
#include "sound.h"
#include "engine.h"
#include "audio.h"

// 4/4 timing
// | 0123 | 4567 | 89ab | cdef |

typedef struct {
    f32 beat_per_second;
    f32 beat_time;
    u32 beat;
    bool beat_changed;


    i32 target_note;
    i32 source_note;
} Music;

static void music_init(Music *music) {
    music->beat_per_second = 120.0f / 60.0f;

    // First beat
    music->beat_changed = true;
}

static void music_play(Music *music, Engine *eng) {
    u32 beat = music->beat;
    if(music->beat_changed) {
        {
        Sound snd = {};
        snd.freq = sound_note_to_freq(-12 * 4);
        snd.src_a.freq = 1;
        snd.src_a.volume = .5;
        snd.src_a.attack_time = 0.0;
        snd.src_a.release_time = 0.5;

        snd.src_b.freq = 1;
        snd.src_b.volume = 8;
        snd.src_b.attack_time = 0.0;
        snd.src_b.release_time = 0.5;
        audio_play(eng->audio, snd);
        }

        if(beat % 4 == 0) {
            music->source_note = music->target_note;
            music->target_note = rand_u32_range(&eng->rng, 0, 12*2) - 12;
        }

        {
            bool kort = beat % 4 > 2;
            i32 note = 0;
            i32 d = music->target_note - music->source_note;
            if(d < 4 && d > -4) d = music->target_note > music->source_note ? 4 : -4;
            if(beat % 4 == 0) note = music->target_note - d / 2;
            if(beat % 4 == 1) note = music->target_note + d / 4;
            if(beat % 4 == 2) note = music->target_note;
            if(beat % 4 == 3) note = music->target_note;

            if(beat % 2 == 0 || kort) {
                Sound snd = {};
                snd.freq = sound_note_to_freq(note - 12);
                snd.src_a.freq = 1;
                snd.src_a.volume = 1;
                snd.src_a.attack_time = 0.01;
                snd.src_a.release_time = kort ? 0.5 : 1.5;

                snd.src_b.freq = 1.0f;
                snd.src_b.volume = 4;
                snd.src_b.attack_time = 0.1;
                snd.src_b.release_time = 8.0;

                // snd.src_c.freq = 4.0f;
                // snd.src_c.volume = 10;
                // snd.src_c.attack_time = 0.5;
                // snd.src_c.release_time = 1.0;
                audio_play(eng->audio, snd);
            }

            if(beat % 2 == 0) {
                Sound snd = {};
                snd.freq = sound_note_to_freq(note - 12*2);
                snd.src_a.freq = 1;
                snd.src_a.volume = 1;
                snd.src_a.attack_time = 0.01;
                snd.src_a.release_time = 1.5;

                snd.src_b.freq = 2.0f;
                snd.src_b.volume = 4;
                snd.src_b.attack_time = 0.1;
                snd.src_b.release_time = 8.0;

                // snd.src_c.freq = 4.0f;
                // snd.src_c.volume = 10;
                // snd.src_c.attack_time = 0.5;
                // snd.src_c.release_time = 1.0;
                audio_play(eng->audio, snd);
            }
        }
    }

    music->beat_changed = false;
    music->beat_time += eng->dt * music->beat_per_second;
    if(music->beat_time >= 1) {
        music->beat_time -= (i32) music->beat_time;
        music->beat++;
        music->beat_changed = true;
    }

    if(music->beat == 16) {
        music->beat = 0;
    }
}
