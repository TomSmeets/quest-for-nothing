// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// build.c - Dynamically compile and reload interactive programs
#include "cli.h"
#include "fmt.h"
#include "hot.h"
#include "mem.h"
#include "os.h"
#include "os_impl.h"
#include "rand.h"
#include "types.h"
#include "watch.h"

#if 0
static void build_linux(bool release) {}
static void build_windows(bool release) {}

static void run() {
    void *cli = 0;

    bool release = false;
    bool watch = false;

    bool run = false;
    u32 run_argc = 0;
    char **run_argv = 0;
    for (;;) {
        if (cli_check(cli, "clean", 0, "Clean build directory")) {
            //
        }

        if (cli_check(cli, "linux", 0, "Build Linux")) {
            build_linux(release);
        }

        if (cli_check(cli, "windows", 0, "Build Windows")) {
            build_windows(release);
        }

        if (cli_check(cli, "all", 0, "Build Everything")) {
            build_linux(release);
            build_windows(release);
        }

        if (cli_check(cli, "release", 0, "Build with optimisations")) release = true;
        if (cli_check(cli, "debug", 0, "Build without optimisations")) release = false;
        if (cli_check(cli, "watch", 0, "Watch for changes and build again")) watch = true;

        if (cli_check(cli, "run", "<main.c> [args]", "Run with hot reloading")) {
            run = true;
            run_argc = cli.argc;
            run_argc = cli.argv;
        }
    }
}
#endif

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
static bool build_single(Memory *tmp, char *output, char *input, Platform plat, bool release, bool dynamic) {
    bool debug = !release;

    fmt_sss(OS_FMT, "Compiling ", input, " to ", output, " in");
    if (dynamic) fmt_s(OS_FMT, " Dynamic");
    fmt_s(OS_FMT, release ? " Release" : " Debug");
    fmt_s(OS_FMT, " mode");
    fmt_s(OS_FMT, " for");
    if (plat == Platform_Linux) fmt_s(OS_FMT, " Linux");
    if (plat == Platform_Windows) fmt_s(OS_FMT, " Windows");
    if (plat == Platform_Wasm) fmt_s(OS_FMT, " WASM");
    fmt_s(OS_FMT, "\n");

    if (plat == Platform_Linux && OS_IS_WINDOWS) {
        fmt_s(OS_FMT, "Cannot cross-compile to linux from windows.\n");
        return true;
    }

    Fmt *fmt = fmt_memory(tmp);

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
#if OS_IS_WINDOWS
        fmt_s(fmt, " -target x86_64-unknown-windows-msvc");
#else
        fmt_s(fmt, " -target x86_64-unknown-windows-gnu");
#endif
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
    if (release) fmt_s(fmt, " -O3");
    if (release && OS_IS_LINUX) fmt_s(fmt, " -Xlinker --strip-all");

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

static void sdl2_download_dll(void) {
    // Skip if already downloaded
    if (os_exists("out/SDL2.dll")) return;

    // Download sdl2
    hot_system("curl -L -o out/SDL2.zip https://github.com/libsdl-org/SDL/releases/download/release-2.30.6/SDL2-2.30.6-win32-x64.zip");
#if OS_IS_WINDOWS
    hot_system("cd out/ && tar -xf SDL2.zip SDL2.dll");
#else
    hot_system("cd out/ && unzip SDL2.zip SDL2.dll");
#endif
}

static bool build_debug(Memory *tmp) {
    if (!build_single(tmp, "out/build", "src/build.c", Platform_Linux, false, false)) return 0;
    if (!build_single(tmp, "out/main", "src/main.c", Platform_Linux, false, false)) return 0;
    return 1;
}

static bool build_release(Memory *tmp, bool release) {
    if (!hot_system("mkdir -p out/release")) return 0;
    if (!build_single(tmp, "out/release/quest-for-nothing.elf", "src/main.c", Platform_Linux, release, false)) return 0;
    if (!build_single(tmp, "out/release/quest-for-nothing.exe", "src/main.c", Platform_Windows, release, false)) return 0;
    if (!build_single(tmp, "out/release/quest-for-nothing.wasm", "src/main.c", Platform_Wasm, release, false)) return 0;

    sdl2_download_dll();
#if OS_IS_WINDOWS
    if (!hot_system("COPY src\\os_wasm.html out\\release\\index.html")) return 0;
    if (!hot_system("COPY out\\SDL2.dll out\\release\\SDL2.dll")) return 0;
#else
    if (!hot_system("cp src/os_wasm.html out/release/index.html")) return 0;
    if (!hot_system("cp out/SDL2.dll out/release/SDL2.dll")) return 0;
#endif
    return 1;
}

// Implementation
// typedef void os_main_t(OS *os);

static char *build_and_load(Memory *tmp, char *main_path, u64 counter) {
    // Generate 'asset.h'

#if OS_IS_WINDOWS
    sdl2_download_dll();
#endif

    Fmt *out_path_fmt = fmt_memory(tmp);
#if OS_IS_WINDOWS
    fmt_su(out_path_fmt, "out/hot-", counter, ".dll");
#else
    fmt_su(out_path_fmt, "/tmp/hot-", counter, ".so");
#endif
    char *out_path = fmt_close(out_path_fmt);
    bool ok = build_single(tmp, out_path, main_path, Platform_Linux, false, true);

    if (!ok) {
        fmt_s(OS_FMT, "Compile error!\n");
        return 0;
    }

    return out_path;
}

typedef struct {
    // Run and hot reload single executable
    bool action_run;
    char *main_path;
    // OS *child_os;
    // os_main_t *update;

    // Build everything
    bool action_build;

    // For generating unique '.so' names
    Random rng;
    Watch watch;
    bool first_time;

    Hot *hot;
} Build;

static void exit_with_help(Cli *cli) {
    char *name = cli->argv[0];
    cli_show_help(cli);
    fmt_s(OS_FMT, "\n");
    fmt_s(OS_FMT, "Examples:\n");
    fmt_ss(OS_FMT, "  ", name, " run src/main.c\n");
    fmt_ss(OS_FMT, "  ", name, " run src/build.c watch\n");
    fmt_ss(OS_FMT, "  ", name, " build\n");
    fmt_ss(OS_FMT, "  ", name, " release\n");
    fmt_ss(OS_FMT, "  ", name, " asset\n");
    os_exit(1);
}

#if OS_IS_LINUX
#include <dirent.h>
#include <stdio.h>
static void include_graph(void) {
    DIR *dir = opendir("src");

    Memory *mem = mem_new();
    Fmt *out = fmt_open(mem, "out/include-graph.dot");
    fmt_s(out, "digraph {\n");
    fmt_s(out, "  layout=dot;\n");
    // fmt_s(out, "  graph[showboxes=2];\n");
    fmt_s(out, "  node[style=filled,fillcolor=\"#ffffff\"];\n");
    // fmt_s(out, "  overlap=false;\n");
    // fmt_s(out, "  rankdir=LR;\n");
    fmt_s(out, "  edge[color=\"#bbbbbb\"];\n");
    // fmt_s(out, "  splines=false;\n");
    // fmt_s(out, "  node[shape=box];\n");
    for (;;) {
        struct dirent *ent = readdir(dir);
        if (!ent) break;

        // Skip '.', '..', and hidden files
        if (ent->d_name[0] == '.') continue;
        if (!(str_ends_with(ent->d_name, ".h") || str_ends_with(ent->d_name, ".c"))) continue;

        // Don't scan opengl api, it is quite big.
        if (str_eq(ent->d_name, "ogl_api.h")) continue;

        // Full Path
        Fmt *full_path_fmt = fmt_memory(mem);
        fmt_ss(full_path_fmt, "src/", ent->d_name, "");
        char *full_path = fmt_close(full_path_fmt);

        // Read file
        FILE *fd = fopen(full_path, "r");

        // Remove .h
        char *input = ent->d_name;
        bool is_c = str_ends_with(input, ".c");
        bool is_h = str_ends_with(input, ".h");
        if (is_c || is_h) input[str_len(input) - 2] = 0;

        u32 line_count = 0;
        u32 dep_count = 0;
        for (;;) {
            char buffer[1024];
            char *line = fgets(buffer, sizeof(buffer), fd);
            if (!line) break;
            line_count++;

            char *prefix = "#include \"";
            char *suffix = "\"\n";
            if (!str_starts_with(line, prefix)) continue;
            if (!str_ends_with(line, suffix)) continue;

            u32 len = str_len(line);
            line[len - str_len(suffix)] = 0;
            line += str_len(prefix);

            // Ignore '../' paths
            if (line[0] == '.') continue;

            // Remove '.c' and '.h'
            if (str_ends_with(line, ".h") || str_ends_with(line, ".c")) {
                line[str_len(line) - 2] = 0;
            }

            dep_count++;
            fmt_sss(out, "  ", input, " -> ", line, ";\n");
        }
        fmt_ss(out, "  ", input, "[");
        f32 size = (f32)f_sqrt(line_count) * 2;
        if (size < 14) size = 14;
        fmt_sf(out, "fontsize=", size, ",");
        if (is_c)
            fmt_s(out, "fillcolor=\"#ffbbbb\",");
        else if (dep_count == 0)
            fmt_s(out, "fillcolor=\"#eeeeff\",");
        fmt_s(out, "];\n");
        fclose(fd);
    }
    fmt_s(out, "}\n");
    fmt_close(out);
    mem_free(mem);
    hot_system("tred out/include-graph.dot > out/include-graph-reduced.dot");
}
#else
static void include_graph(void) {
    os_fail("Not supported on this plaform");
}
#endif

typedef struct {
    u32 argc;
    char **argv;
} Arg;

static char *arg_next(Arg *arg) {
    if (arg->argc == 0) return 0;
    return arg->argc--, *arg->argv++;
}

static Build *build_init(OS *os) {
    Memory *mem = mem_new();
    Memory *tmp = mem_new();
    Build *hot = mem_struct(mem, Build);
    Cli *cli = cli_new(tmp, os);

    // bool watch = false;
    // bool build_linux   = false;
    // bool build_windows = false;
    // bool build_wasm    = false;

    // // build run src/main.c ...
    // // build linux windows wasm
    // // build watch all

    // Arg arg = { os->argc, os->argv };
    // char *main_name = arg_next(&arg);
    // for(;;) {
    //     char *cmd = arg_next(&arg);

    // }

    // for(u32 i = 0; i < os->argc; ++i) {
    //     char *arg = os->argv[i];
    // }

    if (cli_action(cli, "run", "<main> [args]...", "Build and run with hot reloading")) {
        if (os->argc < 3) {
            fmt_s(OS_FMT, "Not enogh arguments\n");
            exit_with_help(cli);
        }

        hot->action_run = true;
        hot->main_path = os->argv[2];
        hot->hot = hot_new(mem, os->argc - 2, os->argv + 2);
    } else if (cli_action(cli, "watch", "", "Build all targets and rebuild on every change")) {
        hot->action_build = true;
    } else if (cli_action(cli, "build", "", "Build for only one target for quick Debugging")) {
        build_debug(tmp);
        os_exit(0);
    } else if (cli_action(cli, "debug", "", "Build all targets in Debug mode")) {
        build_release(tmp, false);
        os_exit(0);
    } else if (cli_action(cli, "release", "", "Build all targets in Release mode")) {
        build_release(tmp, true);
        os_exit(0);
    } else if (cli_action(cli, "upload", "", "Build release and upload to https://tsmeets.itch.io/quest-for-nothing and https://tsmeets.nl/qfn")) {
        if (!build_release(tmp, true)) os_exit(1);
        hot_system("butler push out/release tsmeets/quest-for-nothing:release --userversion $(date +'%F')");
        hot_system("butler push out/release tsmeets/quest-for-nothing:release-web --userversion $(date +'%F')");
        hot_system("rclone copy out/release fastmail:tsmeets.fastmail.com/files/tsmeets.nl/qfn/");
        os_exit(0);
    } else if (cli_action(cli, "serve", "", "Start a simple local python http server for testing the web version")) {
        assert(hot_system("cd out && python -m http.server"), "Failed to start python http server. Is python installed?");
        os_exit(0);
    } else if (cli_action(cli, "asset", "", "Build asset.h")) {
        os_exit(0);
    } else if (cli_action(cli, "format", "", "Format code")) {
        assert(hot_system("clang-format --verbose -i src/*"), "Format failed!");
        os_exit(0);
    } else if (cli_action(cli, "include-graph", "", "Generate Include graph")) {
        include_graph();
        os_exit(0);
    } else {
        exit_with_help(cli);
    }

    // Init inotify
    watch_init(&hot->watch, "src");
    hot->first_time = 1;
    hot->rng.seed = os_rand();
    mem_free(tmp);
    return hot;
}

static void os_main(OS *os) {
    // Call Constructor
    if (!os->app) os->app = build_init(os);

    Build *hot = os->app;
    Memory *tmp = mem_new();

    bool changed = watch_changed(&hot->watch) || hot->first_time;
    hot->first_time = 0;

    // Default update rate
    os->sleep_time = 100 * 1000;

    if (hot->action_run) {
        if (changed) {
            char *so_path = build_and_load(tmp, hot->main_path, rand_u32(&hot->rng));
            hot_load(hot->hot, so_path);
        }

        hot_update(hot->hot);
    }

    if (hot->action_build && changed) {
        build_debug(tmp);
    }
    mem_free(tmp);
}
