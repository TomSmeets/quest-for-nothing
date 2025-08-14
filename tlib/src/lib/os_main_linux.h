// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_main_linux.h - Main entry point for linux
#pragma once
#include "lib/fmt.h"
#include "lib/global.h"
#include "lib/os_api.h"
#include "lib/os_base.h"
#include "lib/os_main_types.h"
#include "lib/rand.h"
#include "lib/std.h"

// Export main, allowing us to dynamically call it
void os_main_dynamic(Global *global_instance) {
    G = global_instance;
    os_main();
}

int main(int argc, char **argv) {
    Global global = {};
    G = &global;

    OS os = {};
    os.argc = argc;
    os.argv = argv;
    G->os = &os;

    Fmt fmt = {};
    fmt.out = fd_to_file(1);
    G->fmt = &fmt;

    Rand rand = rand_new(os_rand());
    G->rand = &rand;

    for (;;) {
        os.sleep_time = 1000 * 1000;
        os_main();
        os_sleep(os.sleep_time);
    }
}
