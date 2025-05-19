// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// build.c - Dynamically compile and reload interactive programs
#include "clang.h"
#include "cli.h"
#include "fmt.h"
#include "fs.h"
#include "hot.h"
#include "include_graph.h"
#include "mem.h"
#include "os_main.h"
#include "rand.h"
#include "tmpfs.h"
#include "types.h"
#include "watch.h"

// Check if file exists
static bool fs_exists(char *path) {
    File *f = os_open(path, Open_Read);
    if (!f) return false;
    os_close(f);
    return true;
}

static void sdl2_download_dll(void) {
    // Skip if already downloaded
    if (fs_exists("out/SDL2.dll")) return;

    // Download sdl2
    os_system("curl -L -o out/SDL2.zip https://github.com/libsdl-org/SDL/releases/download/release-2.30.6/SDL2-2.30.6-win32-x64.zip");
#if OS_IS_WINDOWS
    os_system("cd out/ && tar -xf SDL2.zip SDL2.dll");
#else
    os_system("cd out/ && unzip SDL2.zip SDL2.dll");
#endif
}

static bool build_debug(Memory *tmp) {
    if (!clang_compile(tmp, (Clang_Options){"out/build", "src/build.c", Platform_Linux, false, false})) return 0;
    if (!clang_compile(tmp, (Clang_Options){"out/main", "src/main.c", Platform_Linux, false, false})) return 0;
    return 1;
}

static bool build_release(Memory *tmp, bool release) {
    if (!os_system("mkdir -p out/release")) return 0;
    if (!clang_compile(tmp, (Clang_Options){"out/release/quest-for-nothing.elf", "src/main.c", Platform_Linux, release, false})) return 0;
    if (!clang_compile(tmp, (Clang_Options){"out/release/quest-for-nothing.exe", "src/main.c", Platform_Windows, release, false})) return 0;
    if (!clang_compile(tmp, (Clang_Options){"out/release/quest-for-nothing.wasm", "src/main.c", Platform_Wasm, release, false})) return 0;

    sdl2_download_dll();
#if OS_IS_WINDOWS
    if (!os_system("COPY src\\os_wasm.html out\\release\\index.html")) return 0;
    if (!os_system("COPY out\\SDL2.dll out\\release\\SDL2.dll")) return 0;
#else
    if (!os_system("cp src/os_wasm.html out/release/index.html")) return 0;
    if (!os_system("cp lib/*.js out/release/")) return 0;
    if (!os_system("cp out/SDL2.dll out/release/SDL2.dll")) return 0;
#endif
    return 1;
}

// Implementation
// typedef void os_main_t(OS *os);

static char *build_and_load(Memory *tmp, char *main_path) {
    // Generate 'asset.h'
#if OS_IS_WINDOWS
    sdl2_download_dll();
#endif

#if OS_IS_WINDOWS
    char *out_path = tmpfs_path(tmp, "out/tmp", "hot.dll");
#else
    char *out_path = tmpfs_path(tmp, "out/tmp", "hot.so");
#endif

    bool ok = clang_compile(
        tmp,
        (Clang_Options){
            .output_path = out_path,
            .input_path = main_path,
            .platform = Platform_Linux,
            .release = false,
            .dynamic = true,
        }
    );

    if (!ok) {
        fmt_s(G->fmt, "Compile error!\n");
        return 0;
    }

    return out_path;
}

typedef struct App App;

struct App {
    // Run and hot reload single executable
    bool action_run;
    char *main_path;

    // Build everything
    bool action_build;

    // For generating unique '.so' names
    Rand rng;
    Watch watch;
    bool first_time;

    Hot *hot;
};

static void exit_with_help(Cli *cli) {
    char *name = cli->argv[0];
    cli_show_help(cli);
    fmt_s(G->fmt, "\n");
    fmt_s(G->fmt, "Examples:\n");
    fmt_ss(G->fmt, "  ", name, " run src/main.c\n");
    fmt_ss(G->fmt, "  ", name, " run src/build.c watch\n");
    fmt_ss(G->fmt, "  ", name, " build\n");
    fmt_ss(G->fmt, "  ", name, " release\n");
    fmt_ss(G->fmt, "  ", name, " asset\n");
    os_exit(1);
}

static App *build_init(void) {
    OS *os = G->os;

    Memory *mem = mem_new();
    Memory *tmp = mem_new();
    App *hot = mem_struct(mem, App);
    Cli *cli = cli_new(tmp, os->argc, os->argv);

    if (cli_action(cli, "run", "<main> [args]...", "Build and run with hot reloading")) {
        if (os->argc < 3) {
            fmt_s(G->fmt, "Not enogh arguments\n");
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
        os_system("butler push out/release tsmeets/quest-for-nothing:release --userversion $(date +'%F')");
        os_system("butler push out/release tsmeets/quest-for-nothing:release-web --userversion $(date +'%F')");
        os_system("rclone copy out/release fastmail:tsmeets.fastmail.com/files/tsmeets.nl/qfn/");
        os_exit(0);
    } else if (cli_action(cli, "serve", "", "Start a simple local python http server for testing the web version")) {
        assert(os_system("cd out && python -m http.server"), "Failed to start python http server. Is python installed?");
        os_exit(0);
    } else if (cli_action(cli, "format", "", "Format code")) {
        assert(os_system("clang-format --verbose -i */*.{h,c}"), "Format failed!");
        os_exit(0);
    } else if (cli_action(cli, "include-graph", "", "Generate Include graph")) {
        Include_Graph *graph = include_graph_new(mem);
        include_graph_read_dir(graph, "src", "red");
        include_graph_read_dir(graph, "lib", "blue");
        include_graph_read_dir(graph, "app_build", "green");
        include_graph_read_dir(graph, "app_qfn", "green");
        include_graph_tred(graph);
        // include_graph_rank(graph);
        include_graph_fmt(graph, G->fmt);
        os_exit(0);
    } else {
        exit_with_help(cli);
    }

    // Init inotify
    watch_add(&hot->watch, "lib");
    watch_add(&hot->watch, "src");
    hot->first_time = 1;
    hot->rng = rand_fork(G->rand);
    mem_free(tmp);
    return hot;
}

static void os_main(void) {
    // Call Constructor
    if (!G->app) {
        fs_mkdir("out/tmp");
        tmpfs_clear("out/tmp");
        G->app = build_init();
    }

    App *hot = G->app;
    OS *os = G->os;

    Memory *tmp = mem_new();

    bool changed = watch_check(&hot->watch) || hot->first_time;
    hot->first_time = 0;

    // Default update rate
    os->sleep_time = 100 * 1000;

    if (hot->action_run) {
        if (changed) {
            char *so_path = build_and_load(tmp, hot->main_path);
            hot_load(hot->hot, so_path);
        }

        hot_update(hot->hot);
    }

    if (hot->action_build && changed) {
        build_debug(tmp);
    }
    mem_free(tmp);
}
