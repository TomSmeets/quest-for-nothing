// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_main_linux.h - Main entry point
#pragma once
#include "global.h"
#include "os_api.h"
#include "os_base.h"
#include "std.h"
#include "rand.h"
#include "fmt.h"
#include "os_main_types.h"

// Export main, allowing us to dynamically call it
void os_main_dynamic(Global *global_instance) {
    G = global_instance;
    os_main();
}

// TODO: move back to os_random()
static u64 linux_rand(void) {
    u64 seed = 0;
    i64 ret = linux_getrandom(&seed, sizeof(seed), 0);
    assert(ret == sizeof(seed), "linux getrandom failed");
    return seed;
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

    Rand rand = rand_new(linux_rand());
    G->rand = &rand;

    for (;;) {
        os.sleep_time = 1000 * 1000;
        os_main();
        os_sleep(os.sleep_time);
    }
}
