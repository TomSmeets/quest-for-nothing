// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// sound.h - Audio synthesizer
#pragma once
#include "inc.h"
#include "math.h"
#include "rand.h"

// See [doc/audio.md](doc/audio.md) for information
struct Sound {
    // Time in seconds
    bool play;

    f32 base_volume;

    // Time in seconds
    f32 adsr_attack;
    f32 adsr_decay;
    f32 adsr_sustain;
    f32 adsr_release;

    // volume
    f32 adsr_sustain_level;

    // params
    f32 base_freq;
    f32 lfo_amp;
    f32 lfo_freq;
    f32 compression;
    f32 noise;
    f32 vel;

    // Other Variables
    f32 time;

    rand_t rng;
    f32 t_wave;
    f32 t_lfo;

    bool is_noise;
    f32 o_noise;

    f32 filter;
};

struct Sound_System {
    Sound sounds[32];
};

static Sound *snd_get(Sound_System *sys) {
    for(u32 i =0; i < array_count(sys->sounds); ++i) {
        Sound *snd = sys->sounds + i;
        if(snd->play) continue;
        *snd = (Sound) { 0 };
        return snd;
    }
    return 0;
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

// Triangle wave
static f32 f_triangle(f32 x) {
    f32 v = f_fract(x)*4;
    if(v > 3) return (v - 3) - 1;
    if(v > 1) return 1 - (v - 1);
    return v;
}

// Square wave
static f32 f_square(f32 x) {
    return (f_fract(x) > .5)*2 - 1;
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
static f32 snd_play(Sound *snd, f32 dt) {
    if(!snd->play)
        return 0;

    f32 t_attack = snd->adsr_attack;
    f32 t_decay  = t_attack + snd->adsr_decay;
    f32 t_sustain = t_decay + snd->adsr_sustain;
    f32 t_release = t_sustain + snd->adsr_release;

    if(snd->time > t_release) {
        *snd = (Sound) { 0 };
        return 0;
    }

    f32 volume = snd->base_volume;
    if(0) {}
    else if(snd->time < t_attack) volume *= f_remap(snd->time, 0, t_attack, 0, 1);
    else if(snd->time < t_decay)  volume *= f_remap(snd->time, t_attack, t_decay, 1, snd->adsr_sustain_level);
    else if(snd->time < t_sustain) volume *= snd->adsr_sustain_level;
    else if(snd->time < t_release) volume *= f_remap(snd->time, t_sustain, t_release, snd->adsr_sustain_level, 0);


    f32 o = 0;
    if(snd->is_noise) {
        o = snd->o_noise;
    } else {
        o = f_sin2pi(snd->t_wave + snd->lfo_amp*f_sin2pi(snd->t_lfo));
        o = snd_compress(o, 1 + snd->compression);
    }
    o *= volume;

    // low pass filter
    o = snd->filter += (o - snd->filter)*(dt / (1/(R4*2000) + dt));

    // Advance time
    if(snd->is_noise) {
        snd->t_wave = snd->t_wave + dt*snd->base_freq*(1+ snd->lfo_amp*f_sin2pi(snd->t_lfo));
        if(snd->t_wave > 1) {
            snd->o_noise = (rand_next(&snd->rng) & 1) == 0 ? 1 : -1;
            snd->t_wave = f_fract(snd->t_wave);
        }
    } else {
        snd->t_wave = f_fract(snd->t_wave + dt*snd->base_freq);
    }
    snd->t_lfo  = f_fract(snd->t_lfo  + dt*snd->lfo_freq);

    snd->base_freq += snd->vel*dt;
    if(snd->base_freq < 0) snd->base_freq = 0;

    snd->time += dt;
    return o;
}

static v2 snd_system_play(Sound_System *sys, f32 dt) {
    f32 o = 0;
    for(u32 i = 0; i < array_count(sys->sounds); ++i)
        o += snd_play( &sys->sounds[i], dt);
    return (v2) { o, o };
}
