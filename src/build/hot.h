// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// hot.h - Dynamically reload interactive programs
#pragma once
#include "lib/fmt.h"
#include "lib/global.h"
#include "lib/mem.h"
#include "lib/os_main.h"

typedef void os_main_t(Global *global);

typedef struct {
    os_main_t *child_main;
    App *app;
} Hot;

static Hot *hot_new(Memory *mem) {
    Hot *hot = mem_struct(mem, Hot);
    return hot;
}

// Load child program from path
static bool hot_load(Hot *hot, String path) {
    void *handle = os_dlopen(path);

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
    return 1;
}

#define SWAP(A, B)                                                                                                                                   \
    do {                                                                                                                                             \
        typeof(A) tmp = A;                                                                                                                           \
        A = B;                                                                                                                                       \
        B = tmp;                                                                                                                                     \
    } while (0)

// Call child main function
static void hot_update(Hot *hot, u32 argc, char **argv) {
    if (!hot->child_main) return;
    SWAP(G->app, hot->app);
    SWAP(G->argv, argv);
    SWAP(G->argc, argc);
    hot->child_main(G);
    SWAP(G->app, hot->app);
    SWAP(G->argv, argv);
    SWAP(G->argc, argc);
}
