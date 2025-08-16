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

    // Hot
    Hot *hot;

    // First time?
    bool first;
};

static String hot_fmt(void) {
    Fmt *fmt = fmt_memory(G->tmp);
    fmt_s(fmt, "out/hot_");
    fmt_u(fmt, os_time());
    fmt_s(fmt, ".so");
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

    if (app->build->changed) {
        // Format new output file
        String out_path = hot_fmt();
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

    u32 arg_ix = cli->ix - 1;
    hot_update(app->hot, cli->argc - arg_ix, cli->argv + arg_ix);
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
    os_system(S("mkdir -p out/release/"));

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
        Build *build = build_new();
        build_add_source(build, S("src"));
        build_add_source(build, S("tlib/src"));

        app = G->app = mem_struct(G->mem, App);
        app->build = build;
        app->hot = hot_new(G->mem);
    }

    Cli cli = cli_new(G->argc, G->argv);
    do {
        // Release
        if (build_all(app, &cli)) break;

        // Basics
        if (build_build(app->build, &cli)) break;
        if (build_run(app, &cli)) break;

        // Extras
        if (build_format(app->build, &cli)) break;
        if (build_serve(app->build, &cli)) break;
        if (build_opt_clangd(app->build, &cli)) break;
        if (build_include_graph(app->build, &cli)) break;

        // failed
        cli_show_usage(&cli, G->fmt);
        os_exit(1);
    } while (0);
    build_update(app->build);
}
