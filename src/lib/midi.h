// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// midi.h - Midi file reading and writing
#pragma once
#include "error.h"
#include "fmt.h"
#include "read.h"

// NOTE: (to self) Embrace mem type, just grow fmt, don't care

// https://www.youtube.com/watch?v=P27ml4M3V7A

typedef struct Midi_Track Midi_Track;
struct Midi_Track {
    Midi_Track *next;
};

typedef struct {
    u16 divisions;
    u16 track_count;
    Midi_Track *tracks;
} Midi;

static Midi_Track *midi_read_track(Memory *mem, String *read) {
    if (!read_match(read, S("MTrk"))) return 0;

    u32 size = read_u32be(read);
    String data = read_buf(read, size);

    Midi_Track *track = mem_struct(mem, Midi_Track);
    u8 status = 0;
    for(;;) {
        if(data.len == 0) break;
        u32 dt = read_varint(&data);
        bool has_status = read_peek_u8(&data) >= 0x80;
        if (has_status) {
            status = read_u8(&data);
        }

        u8 event = status >> 4;
        u8 channel = status & 0xf;
        if (event == 0x8) {
            // Note off
            u8 note = read_u8(&data);
            u8 vel = read_u8(&data);
            fmt_su(G->fmt, "U: ", note, "\n");
        } else if (event == 0x9) {
            // Note on
            u8 note = read_u8(&data);
            u8 vel = read_u8(&data);
            fmt_su(G->fmt, "D: ", note, "\n");
        } else if (event == 0xa) {
            // Key Pressure
            u8 note = read_u8(&data);
            u8 pressure = read_u8(&data);
        } else if (event == 0xb) {
            // Control Change
            u8 control = read_u8(&data);
            u8 value = read_u8(&data);
        } else if (event == 0xc) {
            // Program Change
            u8 program = read_u8(&data);
        } else if (event == 0xd) {
            // Channel Pressure
            u8 pressure = read_u8(&data);
        } else if (event == 0xe) {
            // Pitch Wheel
            u16 value = read_u16(&data);
        } else if (event == 0xf) {
            // system event
            if(status == 0xff && read_peek_u8(&data) > 0) {
                // Meta Event
                u8 type = read_u8(&data);
                u32 len = read_varint(&data);
                String msg = read_buf(&data, len);
                fmt_str(G->fmt, S("Meta["));
                fmt_u(G->fmt, type);
                fmt_str(G->fmt, S("]"));
                if(type >= 1 && type <= 7) {
                fmt_str(G->fmt, S("  "));
                    fmt_str(G->fmt, msg);
                }
                fmt_str(G->fmt, S("\n"));
            } else {
                // Control Event
                u32 len = read_varint(&data);
                String msg = read_buf(&data, len);
                // fmt_str(G->fmt, S("Control: "));
                // fmt_str(G->fmt, msg);
                // fmt_str(G->fmt, S("\n"));
            }
        }
    }
    // delta_time
    // status   xx
    // note     0-127
    // velocity 0-127
    return track;
}

static Midi *midi_read(Memory *mem, String *read) {
    // Midi Header
    if (!read_match(read, S("MThd"))) return 0;

    u32 size = read_u32be(read);
    if (size != 3 * 2) return 0;

    u16 format = read_u16be(read);
    u16 track_count = read_u16be(read);
    u16 divisions = read_u16be(read);

    // 0 -> single track
    // 1 -> multi track
    // 2 -> ??? (not supported)
    if (format >= 2) return 0;

    // SMPTE not supported
    if (divisions & (1 << 15)) return 0;

    Midi *midi = mem_struct(mem, Midi);
    midi->divisions = divisions;
    midi->track_count = track_count;

    Midi_Track *last = 0;
    for (u32 i = 0; i < track_count; ++i) {
        Midi_Track *track = midi_read_track(mem, read);
        LIST_APPEND(midi->tracks, last, track);
    }
    return midi;
}

static void test_midi(void) {
    Memory *mem = mem_new();
    String file = os_readfile(mem, S("test.mid"));
    Midi *midi = midi_read(mem, &file);
    assert(midi, "Failed to parse midi");
}
