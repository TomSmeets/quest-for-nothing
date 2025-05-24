// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// music.h - Everything to do with music synthesis
#pragma once
#include "audio.h"
#include "engine.h"
#include "sound.h"
#include "types.h"

typedef struct {
    u8 delta_time;
    u8 status;
    u8 note;
} Note;

typedef struct {
    u32 index;
    u32 count;
    Note notes[64];
} Music;


// Midi note to freq
static f32 music_note_to_freq(u8 note) {
    f32 octave_list[] = {
        1.0f / 32.0f, // -1
        1.0f / 16.0f, // 0
        1.0f / 8.0f,  // 1
        1.0f / 4.0f,  // 2
        1.0f / 2.0f,  // 3
        1.0f,         // 4
        2.0f,         // 5
        4.0f,         // 6
        8.0f,         // 7
        16.0f,        // 8
        32.0f,        // 9
    };

    f32 note_list[] = {
        261.62556530059860, // C
        277.18263097687210, // C#
        293.66476791740760, // D
        311.12698372208090, // D#
        329.62755691286990, // E
        349.22823143300390, // F
        369.99442271163440, // F#
        391.99543598174927, // G
        415.30469757994510, // G#
        440.00000000000000, // A
        466.16376151808990, // A#
        493.88330125612410, // B
    };

    u8 note_ix = note % 12;
    u8 octave_ix = note / 12;
    return octave_list[octave_ix] * note_list[note_ix];
}

static void music_play(Music *music, Engine *eng) {
}
