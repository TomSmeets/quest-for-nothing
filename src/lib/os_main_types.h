// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_main_types.h - Type definitions for os_main
#pragma once
#include "lib/fmt.h"
#include "lib/global.h"
#include "lib/os_desktop.h"
#include "lib/rand.h"
#include "lib/types.h"

// Main callback, implement this method for your application
// os_main is called in a infinite loop, until os_exit is called
// The command line arguments and other members of the `OS` struct
// can be accessed with `G->os`
static void os_main(void);

static Rand *rand_alloc(Memory *mem, u64 seed) {
    Rand *rand = mem_struct(mem, Rand);
    *rand = rand_new(seed);
    return rand;
}

static Global GLOBAL_IMPL;

static void global_init(File *stdout, u64 seed, u32 argc, char **argv) {
    G = &GLOBAL_IMPL;

    Memory *mem = mem_new();
    G->mem = mem;
    G->fmt = fmt_new(mem, stdout);
    G->rand = rand_alloc(mem, seed);
    G->argc = argc;
    G->argv = argv;
    G->dt = 1.0f / 120.0f;
    G->time = os_time();
}

static void global_load(Global *global) {
    G = global;
}

static u64 time_update(u64 *time, u32 *frame_skips, f32 dt) {
    u64 current_time = os_time();
    u64 frame_dt = (u64)(dt * 1e6);
    u64 frame_start = *time;
    u64 frame_end = frame_start + frame_dt;

    if (current_time > frame_end) {
        *time = current_time;
        *frame_skips += 1;
        return 0;
    } else {
        *time = frame_end;
        return frame_end - current_time;
    }
}

static void global_begin(void) {
    G->tmp = mem_new();
}

static u64 global_end(void) {
    mem_free(G->tmp);
    G->tmp = 0;
    return time_update(&G->time, &G->frame_skips, G->dt);
}
