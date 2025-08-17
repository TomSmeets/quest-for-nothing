// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_main_windows.h - Main entry point for Windows
#pragma once
#include "lib/os_main_types.h"

// Export main, allowing us to dynamically call it
__declspec(dllexport) void os_main_dynamic(Global *global) {
    os_main_wrapper(global);
}

// Normal Main entry point, calling os_main statically
int main(int argc, char **argv) {
    Memory *mem = mem_new();
    Global *global = global_new(mem, argc, argv, GetStdHandle(STD_OUTPUT_HANDLE), os_time());
    for (;;) {
        os_main_wrapper(global);
        os_main();
        os_sleep(G->sleep_time);
    }
}
