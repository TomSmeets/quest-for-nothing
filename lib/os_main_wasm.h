// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_main_wasm.h - Main entry point for WASM
#pragma once
#include "os_main_types.h"

static OS G_OS;
static Fmt G_FMT;
static Rand G_RAND;
static Global global;

u64 js_main(void) {
    if (!G) {
        G = &global;
        G->os = &G_OS;
        G_FMT.out = (void *)1;
        G_RAND.seed = os_time();
        G->rand = &G_RAND;
        G->fmt = &G_FMT;
    }
    G->os->sleep_time = 1000 * 1000;
    os_main();
    return G->os->sleep_time;
}
