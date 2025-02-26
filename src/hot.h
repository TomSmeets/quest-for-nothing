// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// hot.h - Dynamically reload interactive programs
#pragma once
#include "fmt.h"
#include "fs.h"
#include "mem.h"
#include "os.h"
#include "global.h"

typedef void os_main_t(Global *global_instance);

typedef struct {
    os_main_t *child_main;
    App *child_app;
    bool reloaded;
} Hot;

static Hot *hot_new(Memory *mem, u32 argc, char **argv) {
    Hot *hot = mem_struct(mem, Hot);
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
    hot->reloaded = true;
    return 1;
}

// Call child main function
static void hot_update(Hot *hot) {
    if (!hot->child_main) return;

    App *host_app = G->app;
    bool host_reloaded = G->reloaded;
    u32 host_argc = G->os->argc;
    char **host_argv = G->os->argv;
    
    G->app = hot->child_app;
    G->os->argc = host_argc - 1;
    G->os->argv = host_argv + 1;
    G->reloaded = hot->reloaded;

    hot->child_main(G);

    hot->child_app = G->app;
    hot->reloaded  = false;

    G->app = host_app;
    G->os->argc = host_argc;
    G->os->argv = host_argv;
    G->reloaded = host_reloaded;
}
