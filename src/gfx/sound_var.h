// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sound_var.h - Immediate mode sound synthesis
#pragma once
#include "gfx/imm.h"
#include "lib/mem.h"
#include "lib/rand.h"
#include "lib/types.h"

#define SOUND_SAMPLE_RATE 48000
#define SOUND_DT (1.0f / SOUND_SAMPLE_RATE)

// ==== SOUND TYPE ====
typedef struct {
    // For noise
    Rand rand;

    // Immediate mode memory
    Imm imm;
} Sound;

static Sound sound_init(Memory *mem) {
    return (Sound){
        .imm = imm_new(mem, 16 * 1024),
    };
}

// Start producing a new sound sample
static void sound_begin(Sound *snd) {
    imm_begin(&snd->imm);
}

// Is this the first iteration?
static bool sound_first(Sound *snd) {
    return imm_first(&snd->imm);
}

#define sound_vars(snd, type, count) imm_array(&(snd)->imm, type, count)
#define sound_var(snd, type) imm_struct(&(snd)->imm, type)
