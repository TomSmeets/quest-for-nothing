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

                            
static void build_all(bool release) {
    // TODO
}

// Implementation
typedef void os_main_t(OS *os);

static os_main_t *build_and_load(char *main_path, u64 counter) {
    // Generate 'asset.h'
    embed_all_assets();

    Memory *tmp = mem_new();
    char *out_path;
    {
        Fmt *fmt = fmt_memory(tmp);
#if OS_IS_WINDOWS
        fmt_su(fmt, "out/hot-", counter, ".dll");
#else
        fmt_su(fmt, "out/hot-", counter, ".so");
#endif
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
    // Run and hot reload single executable
    bool action_run;
    char *main_path;
    OS *child_os;
    os_main_t *update;

    // Build everything
    bool action_build;

    // For generating unique '.so' names
    Random rng;
    Watch watch;
    bool first_time;
} Hot;

static void exit_with_help(OS *os) {
    char *name = os->argv[0];
    fmt_ss(OS_FMT, "Usage: ", name, " <action> [args]...\n");
    fmt_s(OS_FMT, "\n");
    fmt_s(OS_FMT, "Actions:\n");
    fmt_s(OS_FMT, "  run <main> [args]...    Build and run target with hot reloading.\n");
    fmt_s(OS_FMT, "  build                   Build all targets for debugging.\n");
    fmt_s(OS_FMT, "  watch                   Build all targets for debugging on every change.\n");
    fmt_s(OS_FMT, "  release                 Build all targets for relase.\n");
    fmt_s(OS_FMT, "  asset                   Generate code for embedded assets.\n");
    fmt_s(OS_FMT, "  format                  Format code\n");
    fmt_s(OS_FMT, "\n");
    fmt_s(OS_FMT, "Examples:\n");
    fmt_ss(OS_FMT, "  ", name, " run src/main.c\n");
    fmt_ss(OS_FMT, "  ", name, " run src/hot.c watch\n");
    fmt_ss(OS_FMT, "  ", name, " build\n");
    fmt_ss(OS_FMT, "  ", name, " release\n");
    fmt_ss(OS_FMT, "  ", name, " asset\n");
    os_exit(1);
}

static Hot *hot_init(OS *os) {
    Memory *mem = mem_new();
    Hot *hot = mem_struct(mem, Hot);

    if(os->argc < 2) {
        exit_with_help(os);
    }

    char *action = os->argv[1];

    if(str_eq(action, "run")) {
        if (os->argc < 3) {
            fmt_s(OS_FMT, "Not enogh arguments.\n");
            fmt_s(OS_FMT, "\n");
            fmt_s(OS_FMT, "Usage:\n");
            fmt_ss(OS_FMT, "  ", os->argv[0], " run <main> [args]...\n");
            fmt_s(OS_FMT, "\n");
            fmt_s(OS_FMT, "Examples:\n");
            fmt_ss(OS_FMT, "  ", os->argv[0], " run src/main.c\n");
            fmt_ss(OS_FMT, "  ", os->argv[0], " run src/hot.c watch\n");
            os_exit(1);
        }

        hot->action_run = true;
        hot->main_path = os->argv[2];
        hot->child_os = os_init(os->argc - 1, os->argv + 1);
    } else if (str_eq(action, "watch")) {
        hot->action_build = true;
    } else if (str_eq(action, "build")) {
        os_exit(0);
    } else if (str_eq(action, "release")) {
        os_exit(0);
    } else if (str_eq(action, "asset")) {
        embed_all_assets();
        os_exit(0);
    } else if (str_eq(action, "format")) {
        int ret = system("clang-format -i src/*");
        assert(ret == 0, "Format failed!\n");
        os_exit(0);
    } else {
        fmt_ss(OS_FMT, "Invalid action '", action, "'\n\n");
        exit_with_help(os);
    }

    // Init inotify
    watch_init(&hot->watch, "src");
    hot->first_time = 1;
    hot->rng.seed = os_rand();
    return hot;
}

void os_main(OS *os) {
    // Call Constructor
    if (!os->app) os->app = hot_init(os);

    Hot *hot = os->app;

    bool changed = hot->first_time || watch_changed(&hot->watch);
    hot->first_time = 0;

    // Default update rate
    os->sleep_time = 100 * 1000;

    if(hot->action_run) {
        if (changed) {
            hot->update = build_and_load(hot->main_path, rand_u32(&hot->rng));
            hot->child_os->reloaded = 1;
        }

        if (hot->update) {
            hot->update(hot->child_os);

            // inheritchild update rate
            os->sleep_time = hot->child_os->sleep_time;
            hot->child_os->reloaded = 0;
        }
    }

    if (hot->action_build && changed) {
        embed_all_assets();
        fmt_s(OS_FMT, "Build\n");
    }
}
