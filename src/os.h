// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os.h - Base platform API
#pragma once
#include "fmt.h"
#include "global.h"
#include "os_desktop.h"
#include "rand.h"
#include "types.h"

typedef struct OS OS;
struct OS {
    // Command line args
    u32 argc;
    char **argv;

    // Time to sleep until next call
    u64 sleep_time;
};

// Callbacks
static void os_main(void);

// Set maximum wait time between os_main calls
static void os_set_update_time(OS *os, u64 wake_time) {
    if (os->sleep_time > wake_time) {
        os->sleep_time = wake_time;
    }
}

#if OS_IS_LINUX
// Export main, allowing us to dynamically call it
void os_main_dynamic(Global *global_instance) {
    G = global_instance;
    os_main();
}

int main(int argc, char **argv) {
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
#elif OS_IS_WINDOWS
// Export main, allowing us to dynamically call it
__declspec(dllexport) void os_main_dynamic(Global *global_instance) {
    G = global_instance;
    os_main();
}

int main(int argc, char **argv) {
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
#elif OS_IS_WASM
static OS G_OS;
static Fmt G_FMT;
static Rand G_RAND;

u64 js_main(void) {
    if (!G->os) {
        G->os = &G_OS;
        G_FMT.out = (void *)1;
        G_RAND.seed = js_time();
        G->rand = &G_RAND;
        G->fmt = &G_FMT;
    }
    G->os->sleep_time = 1000 * 1000;
    os_main();
    return G->os->sleep_time;
}
#endif
