// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_main_wasm.h - Main entry point for WASM
#pragma once
#include "lib/os_main_types.h"

u64 js_main(void) {
    if (!G) global_init((void *)1, os_time(), 0, 0);
    global_begin();
    os_main();
    return global_end();
}
