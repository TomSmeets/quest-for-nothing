// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_main_linux.h - Main entry point for linux
#pragma once
#include "lib/global.h"
#include "lib/os_base.h"
#include "lib/os_main_types.h"

// Export main, allowing us to dynamically call it
void os_main_dynamic(Global *global) {
    global_load(global);
    os_main();
}

int main(int argc, char **argv) {
    global_init(fd_to_file(1), os_rand(), argc, argv);
    for (;;) {
        global_begin();
        os_main();
        os_sleep(global_end());
    }
}
