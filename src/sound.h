// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// sound.h - Audio synthesizer
#pragma once
#include "inc.h"
#include "math.h"

// See [doc/audio.md](doc/audio.md) for information
// Tracks a number of oscillators
struct sound {
    f32 dt;

    u32 beat;
    f32 bpm;
    f32 beat_time;

    u32 index;
    f32 vars[64];
};

// ==== Base ====
static void snd_begin(sound *snd, f32 dt, f32 bpm) {
    snd->dt  = dt;
    snd->bpm = bpm;
    snd->index = 0;

    // Calculate beat number
    snd->beat_time = snd->beat_time + dt*bpm / 60;
    if(snd->beat_time > 1) {
        snd->beat_time -= 1;
        snd->beat++;
    }
}

// Access a persistent sound variable
static f32 *snd_var(sound *snd) {
    assert(snd->index < array_count(snd->vars));
    return snd->vars + snd->index++;
}

// A simple ramp from 0 to 1 with the given frequency
static f32 snd_ramp(sound *snd, f32 freq) {
    f32 *v = snd_var(snd);
    f32 ret = *v;
    *v = f_fract(*v + snd->dt*freq);
    return ret;
}

// ==== Music ====

// Beats per minute are quite central to music
// Good values are around 120 - 140

struct Note {
    f32 freq;
    u8 vel;
    u8 dur;
};


static char parse_char(char **str) {
    char c = **str;
    if(c == 0)
        return 0;
    (*str)++;
    return c;
}

static Note parse_note(char **note) {
    char cn = parse_char(note);
    char co = parse_char(note);
    char cv = parse_char(note);
    char cd = parse_char(note);

    assert(cn);
    assert(co && co >= '0' && co <= '9');
    assert(cv && cv >= '0' && cv <= '9');
    assert(cd && cd >= '0' && cd <= '9');

    i32 octave  = co - '0';
    u8 velocity = cv - '0';
    u8 duration = cd - '0';

    i32 n = (octave - 4)*12;

    if(0);
    else if(cn == 'c') n += 0;
    else if(cn == 'C') n += 1; // C#
    else if(cn == 'd') n += 2;
    else if(cn == 'D') n += 3; // D#
    else if(cn == 'e') n += 4;
    else if(cn == 'f') n += 5;
    else if(cn == 'F') n += 6;
    else if(cn == 'g') n += 7;
    else if(cn == 'G') n += 8;
    else if(cn == 'a') n += 9;
    else if(cn == 'A') n += 10;
    else if(cn == 'b') n += 11;
    else assert(0);

    return (Note) {
        .freq = 440.0f*f_pow2((f32) (n - 9) / 12.0f),
        .vel = velocity,
        .dur = duration,
    };
}

// ==== Generators ====

// Sine wave
static f32 snd_sin(sound *snd, f32 freq) {
    return f_sin(snd_ramp(snd, freq)*R4);
}

// Square wave
static f32 snd_square(sound *snd, f32 freq, f32 mod) {
    f32 v = snd_ramp(snd, freq)*2;
    return v > 1 + mod ? 1 : -1;
}

// Triangle wave
static f32 snd_tri(sound *snd, f32 freq) {
    f32 v = snd_ramp(snd, freq)*4;
    if(v > 3) return (v - 3) - 1;
    if(v > 1) return 1 - (v - 1);
    return v;
}

static f32 f_square(f32 x) {
    return f_fract(x) > .5;
}

// ==== Effects ====
static f32 snd_compress(f32 v, f32 amount) {
    return f_clamp(v*amount, -1, 1);
}

static f32 snd_cut(f32 v, f32 amount) {
    if(v > 0) return f_max(v - amount, 0);
    if(v < 0) return f_min(v + amount, 0);
    return 0;
}

// ==== Filters ====
