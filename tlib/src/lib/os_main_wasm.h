// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_main_wasm.h - Main entry point for WASM
#pragma once
#include "lib/os_main_types.h"

u64 js_main(void) {
    if (!G) os_main_init(0, 0, (void *)1, os_time());
    os_main_begin();
    os_main();
    return G->os->sleep_time;
}
