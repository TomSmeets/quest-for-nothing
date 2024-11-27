// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// hot.c - Dynamically compile and reload interactive programs
//
// Usage: ./hot src/main.c [ARGS]...
#include "embed.h"
#include "fmt.h"
#include "mem.h"
#include "os.h"
#include "os_api.h"
#include "rand.h"
#include "types.h"
#include "watch.h"

static const char *compile_command = "clang"
                                     // Enable most warning flags
                                     " -Wall -Werror"
                                     " -Wno-unused-function"
                                     " -Wno-unused-variable"
                                     " -Wno-unused-but-set-variable"
                                     " -Wno-format"
                                     // We are running on this cpu
                                     " -march=native"
                                     // Don't optimize, quick compile times
                                     " -O0 -g"
                                     " -std=c23"
                                     // Create a '.so' file for dynamic loading
                                     " -shared";

// Implementation
typedef void os_main_t(OS *os);

static os_main_t *build_and_load(char *main_path, u64 counter) {
    // Generate 'asset.h'
    embed_all_assets();

    Memory *tmp = mem_new();
    char *out_path;
    {
        Fmt *fmt = fmt_memory(tmp);
        fmt_su(fmt, "out/hot-", counter, ".so");
        out_path = fmt_close(fmt);
    }

    char *command;
    {
        Fmt *fmt = fmt_memory(tmp);
        fmt_s(fmt, (char *)compile_command);
        fmt_ss(fmt, " -o ", out_path, "");
        fmt_ss(fmt, " ", main_path, "");
        command = fmt_close(fmt);
    }

    fmt_ss(OS_FMT, "Running: ", command, "\n");
    int ret = system(command);
    assert(ret >= 0, "Error while compiling");

    mem_free(tmp);

    if (ret != 0) {
        fmt_s(OS_FMT, "Compile error!\n");
        return 0;
    }

    void *handle = os_dlopen(out_path);
    if (!handle) {
        fmt_ss(OS_FMT, "dlopen: ", os_dlerror(), "\n");
        return 0;
    }

    os_main_t *fcn = os_dlsym(handle, "os_main_dynamic");
    if (!fcn) {
        fmt_ss(OS_FMT, "dlsym: ", os_dlerror(), "\n");
        return 0;
    }

    return fcn;
}

typedef struct {
    // For generating unique '.so' names
    Random rng;

    // main.c
    char *main_path;

    // OS struct for the child
    OS *child_os;

    // File watch
    Watch watch;

    // Update function
    bool first_time;
    os_main_t *update;
} Hot;

static Hot *hot_load(OS *os) {
    // Already loaded
    if (os->app) return os->app;

    Memory *mem = mem_new();
    Hot *hot = mem_struct(mem, Hot);

    // Parse arguments
    if (os->argc < 2) {
        fmt_ss(OS_FMT, "", os->argv[0], " <MAIN_FILE> [ARGS]...\n");
        os_exit(1);
    }

    hot->main_path = os->argv[1];

    // Prepare OS handle for the child
    hot->child_os = os_init(os->argc - 1, os->argv + 1);

    // Init inotify
    watch_init(&hot->watch);
    watch_add(&hot->watch, ".");
    watch_add(&hot->watch, "src");

    hot->first_time = 1;

    // Init rng, nested 'hot' would not work otherwise
    hot->rng.seed = os_rand();

    // Save to os
    os->app = hot;
    return hot;
}

void os_main(OS *os) {
    Hot *hot = hot_load(os);

    if (hot->first_time || watch_changed(&hot->watch)) {
        hot->update = build_and_load(hot->main_path, rand_u32(&hot->rng));
        hot->child_os->reloaded = 1;
        hot->first_time = 0;
    }

    if (hot->update) {
        hot->update(hot->child_os);
        os->sleep_time = hot->child_os->sleep_time;
        hot->child_os->reloaded = 0;
    } else {
        os->sleep_time = 100 * 1000;
    }
}
