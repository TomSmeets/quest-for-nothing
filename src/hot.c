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

// Target platform
typedef enum {
    Platform_Linux,
    Platform_Windows,
    Platform_Wasm,
} Platform;

static bool hot_system(char *cmd) {
    fmt_ss(OS_FMT, "> ", cmd, "\n");

    int ret = system(cmd);
    if (ret != 0) {
        fmt_s(OS_FMT, "Command Failed!\n");
    }
    return ret == 0;
}

// Build single executable using clang
// This will become a single 'clang' call
static bool build_single(Memory *mem, char *output, char *input, Platform plat, bool release, bool dynamic) {
    bool debug = !release;

    Fmt *fmt = fmt_memory(mem);

    fmt_s(fmt, "clang");

    // Configure warning flags
    fmt_s(fmt, " -Wall -Werror");
    fmt_s(fmt, " -Wno-unused-function");
    fmt_s(fmt, " -Wno-unused-variable");
    fmt_s(fmt, " -Wno-unused-but-set-variable");
    fmt_s(fmt, " -Wno-format");

    fmt_s(fmt, " -std=c23");

    // We are running on this cpu
    // https://pkgstats.archlinux.de/compare/system-architectures/x86_64
    if (plat != Platform_Wasm) {
        if (debug) fmt_s(fmt, " -march=native");
        if (release) fmt_s(fmt, " -march=x86-64-v3");
    }

    if (plat == Platform_Windows) {
        fmt_s(fmt, " -target x86_64-unknown-windows-gnu");
    }

    if (plat == Platform_Wasm) {
        fmt_s(fmt, " -target wasm32");
        fmt_s(fmt, " --no-standard-libraries");
        fmt_s(fmt, " -Wl,--no-entry");
        fmt_s(fmt, " -Wl,--export-all");
        fmt_s(fmt, " -fno-builtin");
        fmt_s(fmt, " -msimd128");
    }

    // Don't optimize, quick compile times
    if (debug) fmt_s(fmt, " -O0 -g");
    if (release) fmt_s(fmt, " -O3 -Xlinker --strip-all");

    // Create a '.so' file for dynamic loading
    if (dynamic) fmt_s(fmt, " -shared");

    fmt_ss(fmt, " -o ", output, "");
    fmt_ss(fmt, " ", input, "");

    char *cmd = fmt_close(fmt);
    bool ok = hot_system(cmd);
    return ok;
}

// Check if file exists
static bool os_exists(char *path) {
    File *f = os_open(path, Open_Read);
    if (!f) return false;
    os_close(f);
    return true;
}

static void sdl2_download_dll(Memory *mem) {
    // Skip if already downloaded
    if (os_exists("out/SDL2.dll")) return;

#if OS_IS_WINDOWS
    fmt_s(OS_FMT, "Please download SDL2.dll into the 'out/' directory\n");
    fmt_s(OS_FMT, "Download here: https://github.com/libsdl-org/SDL/releases/download/release-2.30.6/SDL2-2.30.6-win32-x64.zip\n");
#else
    // Download sdl2
    hot_system("curl -L 'https://github.com/libsdl-org/SDL/releases/download/release-2.30.6/SDL2-2.30.6-win32-x64.zip' -o out/SDL2.zip");
    hot_system("unzip -o out/SDL2.zip SDL2.dll -d out");
#endif
}

static bool build_all(Memory *mem, bool release) {
    if (!build_single(mem, "out/hot", "src/hot.c", Platform_Linux, release, false)) return 0;
    if (!build_single(mem, "out/hot", "src/hot.c", Platform_Windows, release, false)) return 0;

    embed_all_assets();
    sdl2_download_dll(mem);
    if (!build_single(mem, "out/main.elf", "src/main.c", Platform_Linux, release, false)) return 0;
    if (!build_single(mem, "out/main.exe", "src/main.c", Platform_Windows, release, false)) return 0;
    if (!build_single(mem, "out/main.wasm", "src/main.c", Platform_Wasm, release, false)) return 0;

    if (!hot_system("cp src/os_wasm.html out/index.html")) {
        fmt_s(OS_FMT, "Failed to copy\n");
        return 0;
    }

    return 1;
}

// Implementation
typedef void os_main_t(OS *os);

static os_main_t *build_and_load(Memory *mem, char *main_path, u64 counter) {
    // Generate 'asset.h'
    embed_all_assets();

#if OS_IS_WINDOWS
    sdl2_download_dll(mem);
#endif

    Fmt *out_path_fmt = fmt_memory(mem);
#if OS_IS_WINDOWS
    fmt_su(out_path_fmt, "out/hot-", counter, ".dll");
#else
    fmt_su(out_path_fmt, "out/hot-", counter, ".so");
#endif
    char *out_path = fmt_close(out_path_fmt);
    bool ok = build_single(mem, out_path, main_path, Platform_Linux, false, true);

    if (!ok) {
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
    Memory *tmp = mem_new();
    Hot *hot = mem_struct(mem, Hot);

    if (os->argc < 2) {
        exit_with_help(os);
    }

    char *action = os->argv[1];

    if (str_eq(action, "run")) {
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
        build_all(tmp, false);
        os_exit(0);
    } else if (str_eq(action, "release")) {
        build_all(tmp, true);
        os_exit(0);
    } else if (str_eq(action, "asset")) {
        embed_all_assets();
        os_exit(0);
    } else if (str_eq(action, "format")) {
        assert(hot_system("clang-format -i src/*"), "Format failed!\n");
        os_exit(0);
    } else {
        fmt_ss(OS_FMT, "Invalid action '", action, "'\n\n");
        exit_with_help(os);
    }

    // Init inotify
    watch_init(&hot->watch, "src");
    hot->first_time = 1;
    hot->rng.seed = os_rand();
    mem_free(tmp);
    return hot;
}

void os_main(OS *os) {
    // Call Constructor
    if (!os->app) os->app = hot_init(os);

    Hot *hot = os->app;
    Memory *tmp = mem_new();

    bool changed = hot->first_time || watch_changed(&hot->watch);
    hot->first_time = 0;

    // Default update rate
    os->sleep_time = 100 * 1000;

    if (hot->action_run) {
        if (changed) {
            hot->update = build_and_load(tmp, hot->main_path, rand_u32(&hot->rng));
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
        build_all(tmp, false);
    }
    mem_free(tmp);
}
