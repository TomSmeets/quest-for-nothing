// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// music.h - Everything to do with music synthesis
#pragma once
#include "audio.h"
#include "engine.h"
#include "midi.h"
#include "sound.h"
#include "types.h"

typedef struct {
    u32 index;
    u64 time;
} Music_Track;

typedef struct {
    Midi *midi;
    Music_Track tracks[32];
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
    if (!music->midi) {
        *music = (Music){};
        Memory *mem = mem_new();
        String file = os_readfile(mem, S("test3.mid"));
        music->midi = midi_read(mem, &file);
        assert0(music->midi);
    }

    f32 speed = 0.35f;
    u32 track_ix = 0;
    bool all_done = true;
    for (Midi_Track *track = music->midi->tracks; track; track = track->next) {
        assert0(track_ix < array_count(music->tracks));
        Music_Track *mtrack = music->tracks + track_ix++;

        for (;;) {
            if (mtrack->index == track->note_count) break;
            if (track->note_count == 0) break;
            all_done = false;

            Midi_Note *note = track->notes + mtrack->index;
            u64 dt = note->time;
            // fmt_su(G->fmt, "Index: ", music->index, "\n");
            // fmt_su(G->fmt, "Time: ", music->time, "\n");
            if (mtrack->time < dt) break;
            mtrack->index++;
            mtrack->time -= dt;

            if (!note->down) continue;

            u32 duration = 0;
            for (u32 i = mtrack->index; i < track->note_count; ++i) {
                Midi_Note *next = track->notes + i;
                duration += next->time;
                if (next->note == note->note && !next->down) break;
            }

            fmt_su(G->fmt, "Track: ", track_ix, " ");
            fmt_su(G->fmt, "Ix: ", mtrack->index, " ");
            fmt_su(G->fmt, "Note: ", note->note, " ");
            fmt_su(G->fmt, "Vel: ", note->vel, " ");
            fmt_su(G->fmt, "Duration: ", duration, "\n");

            Voice voice = {
                .freq = music_note_to_freq(note->note),
                .time = 0,
                .kind = 2,
                .velocity = note->vel,
                .duration = duration / 1000.0f * 8 / speed,
            };
            audio_play(eng->audio, voice);
        }
        mtrack->time += eng->dt * 1000 * speed;
    }

    if (all_done) {
        std_memzero((u8 *)music->tracks, sizeof(music->tracks));
    }
}
