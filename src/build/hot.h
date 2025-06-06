// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// hot.h - Dynamically reload interactive programs
#pragma once
#include "lib/fmt.h"
#include "lib/global.h"
#include "lib/mem.h"
#include "lib/os_main.h"

typedef void os_main_t(Global *global_instance);

typedef struct {
    os_main_t *child_main;
    OS child_os;
    Global child_global;
} Hot;

static Hot *hot_new(Memory *mem, u32 argc, char **argv) {
    Hot *hot = mem_struct(mem, Hot);

    // Copy our global
    hot->child_os.argc = argc;
    hot->child_os.argv = argv;
    hot->child_global.os = &hot->child_os;

    // Share
    hot->child_global.fmt = G->fmt;
    hot->child_global.rand = G->rand;

    return hot;
}

// Load child program from path
static bool hot_load(Hot *hot, char *path) {
    void *handle = os_dlopen(str_from(path));

    if (!handle) {
        fmt_ss(G->fmt, "dlopen: ", os_dlerror(), "\n");
        return 0;
    }

    os_main_t *child_main = os_dlsym(handle, S("os_main_dynamic"));
    if (!child_main) {
        fmt_ss(G->fmt, "dlsym: ", os_dlerror(), "\n");
        return 0;
    }

    hot->child_main = child_main;
    hot->child_global.reloaded = true;
    return 1;
}

// Call child main function
static void hot_update(Hot *hot) {
    hot->child_os.sleep_time = G->os->sleep_time;
    if (!hot->child_main) return;
    hot->child_main(&hot->child_global);
    hot->child_global.reloaded = false;
    G->os->sleep_time = hot->child_global.os->sleep_time;
}
