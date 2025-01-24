// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// hot.h - Dynamically reload interactive programs
#pragma once
#include "os.h"
#include "fmt.h"
#include "mem.h"

typedef void os_main_t(OS *os);

typedef struct {
    OS *child_os;
    os_main_t *child_main;
} Hot;

static Hot *hot_new(Memory *mem, u32 argc, char **argv) {
    Hot *hot = mem_struct(mem, Hot);

    OS *child_os = mem_struct(mem, OS);
    child_os->argc = argc;
    child_os->argv = argv;
    child_os->fmt = OS_GLOBAL->fmt;
    hot->child_os = child_os;

    return hot;
}

// Load child program from path
static bool hot_load(Hot *hot, char *path) {
    void *handle = os_dlopen(path);

    if (!handle) {
        fmt_ss(OS_FMT, "dlopen: ", os_dlerror(), "\n");
        return 0;
    }

    os_main_t *child_main = os_dlsym(handle, "os_main_dynamic");
    if (!child_main) {
        fmt_ss(OS_FMT, "dlsym: ", os_dlerror(), "\n");
        return 0;
    }

    hot->child_main = child_main;
    hot->child_os->reloaded = 1;
    return 1;
}

static void os_set_update_time(u64 wake_time) {
    OS *os = OS_GLOBAL;
    if(os->sleep_time > wake_time) {
        os->sleep_time = wake_time;
    }
}

// Call child main function
static void hot_update(Hot *hot) {
    OS *os = OS_GLOBAL;
    if(!hot->child_main) return;

    hot->child_main(hot->child_os);

    // Reset OS_GLBOAL
    assert(OS_GLOBAL == os, "OS should not have changed.. right?");

    // inherit child update rate
    os_set_update_time(hot->child_os->sleep_time);
    hot->child_os->reloaded = 0;
}
