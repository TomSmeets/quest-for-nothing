// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_main_windows.h - Main entry point for Windows
#pragma once
#include "os_main_types.h"

// Export main, allowing us to dynamically call it
__declspec(dllexport) void os_main_dynamic(Global *global_instance) {
    G = global_instance;
    os_main();
}

// Normal Main entry point, calling os_main statically
int main(int argc, char **argv) {
    Global global = {};
    G = &global;

    OS os = {};
    os.argc = argc;
    os.argv = argv;
    G->os = &os;

    Fmt fmt = {};
    fmt.out = GetStdHandle(STD_OUTPUT_HANDLE);
    G->fmt = &fmt;

    Rand rand = rand_new(os_time());
    G->rand = &rand;
    for (;;) {
        os.sleep_time = 1000 * 1000;
        os_main();
        os_sleep(os.sleep_time);
    }
}
