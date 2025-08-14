// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// build.c - Build tool for runing and building applications
#include "build/build.h"
#include "build/clang.h"
#include "build/cli.h"
#include "build/hot.h"
#include "build/include_graph.h"
#include "build/watch.h"
#include "lib/fmt.h"
#include "lib/os_main.h"

struct App {
    Build *build;

    // Build
    bool do_build;
    Clang_Options build_opts;

    // Hot
    Hot *hot;
    String hot_output;
    Fmt hot_output_fmt;

    // First time?
    bool first;
};

static bool build_include_graph(App *app, Cli *cli) {
    bool build = cli_flag(cli, "include-graph", "Generate Include graph");
    if (!build) return false;
    Include_Graph *graph = include_graph_new(G->tmp);
    include_graph_read_dir(graph, S("src"));
    include_graph_read_dir(graph, S("tlib/src"));
    include_graph_tred(graph);
    // include_graph_rank(graph);
    include_graph_fmt(graph, G->fmt);
    os_exit(0);
    return true;
}

// Reset formatter and create a unique output path for a .so
static String hot_fmt(Fmt *fmt) {
    fmt->used = 0;
    fmt_su(fmt, "out/hot_", os_time(), ".so");
    return fmt_get(fmt);
}

static bool build_run(App *app, Cli *cli) {
    // Check command
    if (!cli_flag(cli, "run", "Run an application with dynamic hot reloading")) return false;

    char *input_path = cli_value(cli, "<INPUT>", "Input file");

    if (!input_path) {
        cli_show_usage(cli, G->fmt);
        os_exit(1);
    }

    // Init hot (if first time)
    if (!app->hot) {
        char **hot_argv = cli->argv + cli->ix - 1;
        u32 hot_argc = cli->argc - cli->ix + 1;
        app->hot = hot_new(G->mem, hot_argc, hot_argv);
    }

    if (app->build->changed) {
        // Remove previous output file
        if (app->hot_output.len) {
            fs_remove(app->hot_output);
        }

        // Format new output file
        String out_path = hot_fmt(&app->hot_output_fmt);
        app->hot_output = out_path;
        fmt_s(G->fmt, "OUT: ");
        fmt_str(G->fmt, out_path);
        fmt_s(G->fmt, "\n");

        Clang_Options opts = {
            .input_path = input_path,
            .output_path = (char *)out_path.data,
            .dynamic = true,
        };

        if (clang_compile(opts)) {
            hot_load(app->hot, out_path);
        } else {
            app->hot->child_main = 0;
        }
    }

    hot_update(app->hot);
    return true;
}

static bool fs_exists(String path) {
    File *f = os_open(path, Open_Read);
    if (!f) return false;
    os_close(f);
    return true;
}

static bool build_all(App *app, Cli *cli) {
    bool build = cli_flag(cli, "release", "Build qfn release");
    bool upload = cli_flag(cli, "publish", "Upload archive");
    if (!build && !upload) return false;

    Clang_Options opts = {};
    opts.input_path = "src/qfn/qfn.c";
    opts.release = true;

    opts.platform = Platform_Linux;
    opts.output_path = "out/release/quest_for_nothing.elf";
    if (!clang_compile(opts)) os_exit(1);

    opts.platform = Platform_Windows;
    opts.output_path = "out/release/quest_for_nothing.exe";
    if (!clang_compile(opts)) os_exit(1);

    opts.platform = Platform_Wasm;
    opts.output_path = "out/release/quest_for_nothing.wasm";
    if (!clang_compile(opts)) os_exit(1);

    // Download SDL3
    if (!fs_exists(S("out/SDL3.dll"))) {
        os_system(S("curl -L -o out/SDL3.zip https://github.com/libsdl-org/SDL/releases/download/release-3.2.16/SDL3-3.2.16-win32-x64.zip"));
        os_system(S("cd out/ && unzip SDL3.zip SDL3.dll"));
    }
    os_system(S("cp out/SDL3.dll out/release/SDL3.dll"));
    os_system(S("cp tlib/src/lib/*.js out/release/"));
    os_system(S("cp tlib/src/gfx/*.js out/release/"));
    os_system(S("cp src/qfn/index.html out/release/index.html"));

    if (upload) {
        os_system(S("butler push out/release tsmeets/quest-for-nothing:release --userversion $(date +'%F')"));
        os_system(S("butler push out/release tsmeets/quest-for-nothing:release-web --userversion $(date +'%F')"));
        os_system(S("rclone copy out/release fastmail:tsmeets.fastmail.com/files/tsmeets.nl/qfn/"));
    }
    os_exit(0);
    return true;
}

static void os_main(void) {
    App *app = G->app;

    if (!app) {
        app = G->app = mem_struct(G->mem, App);
        app->build = build_new();
        app->hot_output_fmt.mem = G->mem;
        build_add_source(app->build, S("src"));
        build_add_source(app->build, S("tlib/src"));
    }

    Cli cli = cli_new(G->os->argc, G->os->argv);
    do {
        // Release
        if (build_all(app, &cli)) break;

        // Basics
        if (build_build(app->build, &cli)) break;
        if (build_run(app, &cli)) break;

        // Extras
        if (build_format(app->build, &cli)) break;
        if (build_serve(app->build, &cli)) break;
        if (build_include_graph(app, &cli)) break;

        // failed
        cli_show_usage(&cli, G->fmt);
        os_exit(1);
    } while (0);
    build_update(app->build);
}
