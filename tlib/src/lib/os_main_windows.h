// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_main_windows.h - Main entry point for Windows
#pragma once
#include "lib/os_main_types.h"

// Export main, allowing us to dynamically call it
__declspec(dllexport) void os_main_dynamic(Global *global) {
    global_load(global);
    os_main();
}

// Normal Main entry point, calling os_main statically
int main(int argc, char **argv) {
    global_init(GetStdHandle(STD_OUTPUT_HANDLE), os_time(), argc, argv);
    for (;;) {
        global_begin();
        os_main();
        u64 sleep_time = global_end(); 
        fmt_s(G->fmt, "Sleep: ");
        fmt_u(G->fmt, sleep_time);
        fmt_s(G->fmt, "\n");
        os_sleep(sleep_time);
    }
}
