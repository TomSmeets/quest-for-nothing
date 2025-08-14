// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_main_types.h - Type definitions for os_main
#pragma once
#include "lib/fmt.h"
#include "lib/global.h"
#include "lib/os_desktop.h"
#include "lib/rand.h"
#include "lib/types.h"

typedef struct OS OS;
struct OS {
    // Command line args
    u32 argc;
    char **argv;

    // Time to sleep until the next time
    // os_main is called.
    u64 sleep_time;
};

static Global GLOBAL_IMPL;

static void os_main_init(u32 argc, char **argv, File *stdout, u64 seed) {
    G = &GLOBAL_IMPL;
    G->mem = mem_new();
    G->os = mem_struct(G->mem, OS);
    G->os->argc = argc;
    G->os->argv = argv;
    G->fmt = fmt_new(G->mem, stdout);
    G->rand = mem_struct(G->mem, Rand);
    G->rand->seed = seed;
    G->os->sleep_time = 1000 * 1000;
}

static void os_main_begin(void) {
    if (G->tmp) mem_free(G->tmp);
    G->tmp = mem_new();
    G->os->sleep_time = 1000 * 1000;
}

// Main callback, implement this method for your application
// os_main is called in a infinite loop, until os_exit is called
// The command line arguments and other members of the `OS` struct
// can be accessed with `G->os`
static void os_main(void);
