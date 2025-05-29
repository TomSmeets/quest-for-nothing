// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sound_music.h - Everything for nice music
#pragma once
#include "sound_var.h"

// Octave
#define OCT_N1 (1.0f / 32.0f)
#define OCT_0 (1.0f / 16.0f)
#define OCT_1 (1.0f / 8.0f)
#define OCT_2 (1.0f / 4.0f)
#define OCT_3 (1.0f / 2.0f)
#define OCT_4 (1.0f)
#define OCT_5 (2.0f)
#define OCT_6 (4.0f)
#define OCT_7 (8.0f)
#define OCT_8 (16.0f)
#define OCT_9 (16.0f)

// Note
#define NOTE_C 261.62556530059860
#define NOTE_CS 277.18263097687210
#define NOTE_D 293.66476791740760
#define NOTE_DS 311.12698372208090
#define NOTE_E 329.62755691286990
#define NOTE_F 349.22823143300390
#define NOTE_FS 369.99442271163440
#define NOTE_G 391.99543598174927
#define NOTE_GS 415.30469757994510
#define NOTE_A 440.00000000000000
#define NOTE_AS 466.16376151808990
#define NOTE_B 493.88330125612410

// Midi note to freq
static f32 sound_note(u8 note) {
    f32 octave_list[] = {
        OCT_N1, OCT_0, OCT_1, OCT_2, OCT_3, OCT_4, OCT_5, OCT_6, OCT_7, OCT_8, OCT_9,
    };

    f32 note_list[] = {
        NOTE_C,  // C
        NOTE_CS, // C#
        NOTE_D,  // D
        NOTE_DS, // D#
        NOTE_E,  // E
        NOTE_F,  // F
        NOTE_FS, // F#
        NOTE_G,  // G
        NOTE_GS, // G#
        NOTE_A,  // A
        NOTE_AS, // A#
        NOTE_B,  // B
    };

    u8 note_ix = note % array_count(note_list);
    u8 octave_ix = note / array_count(note_list);
    return octave_list[octave_ix] * note_list[note_ix];
}

// Midi note to freq
static f32 sound_scale(u8 note) {
    f32 octave_list[] = {
        OCT_N1, OCT_0, OCT_1, OCT_2, OCT_3, OCT_4, OCT_5, OCT_6, OCT_7, OCT_8, OCT_9,
    };

    f32 note_list[] = {
        NOTE_C,  // C
        NOTE_D,  // D
        NOTE_E,  // E
        NOTE_F,  // F
        NOTE_G,  // G
        NOTE_A,  // A
        NOTE_B,  // B
    };

    u8 note_ix = note % array_count(note_list);
    u8 octave_ix = note / array_count(note_list);
    return octave_list[octave_ix] * note_list[note_ix];
}
