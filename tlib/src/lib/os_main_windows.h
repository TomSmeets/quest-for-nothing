// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_main_windows.h - Main entry point for Windows
#pragma once
#include "lib/os_main_types.h"

// Export main, allowing us to dynamically call it
__declspec(dllexport) void os_main_dynamic(Global *global_instance) {
    G = global_instance;
    os_main();
}

// Normal Main entry point, calling os_main statically
int main(int argc, char **argv) {
    os_main_init(argc, argv, GetStdHandle(STD_OUTPUT_HANDLE), os_time());
    for (;;) {
        os_main_begin();
        os_main();
        os_sleep(G->os->sleep_time);
    }
}
