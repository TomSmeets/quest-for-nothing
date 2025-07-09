// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// build.c - Build tool for runing and building applications
#include "build/clang.h"
#include "build/cli.h"
#include "build/hot.h"
#include "build/include_graph.h"
#include "build/watch.h"
#include "lib/fmt.h"
#include "lib/os_main.h"

struct App {
    // Permanent memory
    Memory *mem;

    // Per-frame memory
    Memory *tmp;

    Watch watch;

    // Build
    bool do_build;
    Clang_Options build_opts;

    // Hot
    Hot *hot;
    String hot_output;
    Fmt hot_output_fmt;

    // First time?
    bool first;
    bool changed;
};

static bool build_read_opts(Cli *cli, Clang_Options *opts) {
    opts->input_path = cli_value(cli, "<Input>", "Source Input Path");
    if (!opts->input_path) return false;

    opts->output_path = cli_value(cli, "<Output>", "Execurtable Output Path");
    if (!opts->output_path) return false;

    while (cli_read(cli)) {
        if (0) {
        } else if (cli_flag(cli, "linux", "Compile for Linux")) {
            opts->platform = Platform_Linux;
        } else if (cli_flag(cli, "windows", "Compile for Windows")) {
            opts->platform = Platform_Windows;
        } else if (cli_flag(cli, "wasm", "Compile for Webassembly")) {
            opts->platform = Platform_Wasm;
        } else if (cli_flag(cli, "release", "Compile in Release Mode")) {
            opts->release = true;
        } else if (cli_flag(cli, "dynamic", "Create a dynamic executable (a .dll/.so file)")) {
            opts->dynamic = true;
        } else {
            return false;
        }
    }
    return true;
}

static bool build_format(App *app, Cli *cli) {
    if (!cli_flag(cli, "format", "Run code formatter")) return false;
    bool result = os_system(S("clang-format --verbose -i src/*/*.{h,c}"));
    assert(result, "Format failed!");
    os_exit(0);
    return true;
}

static bool build_include_graph(App *app, Cli *cli) {
    bool build = cli_flag(cli, "include-graph", "Generate Include graph");
    if (!build) return false;
    Include_Graph *graph = include_graph_new(app->tmp);
    include_graph_read_dir(graph, S("src"));
    include_graph_tred(graph);
    // include_graph_rank(graph);
    include_graph_fmt(graph, G->fmt);
    os_exit(0);
    return true;
}

// Reset formatter and create a unique output path for a .so
static char *hot_fmt(Fmt *fmt) {
    fmt->used = 0;
    fmt_su(fmt, "out/hot_", os_time(), ".so");
    return fmt_close(fmt);
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
        app->hot = hot_new(app->mem, hot_argc, hot_argv);
    }

    if (app->changed) {
        // Remove previous output file
        if (app->hot_output.len) {
            fs_remove(app->hot_output);
        }

        // Format new output file
        char *out_path = hot_fmt(&app->hot_output_fmt);
        app->hot_output = str_from(out_path);
        fmt_ss(G->fmt, "OUT: ", out_path, "\n");

        Clang_Options opts = {
            .input_path = input_path,
            .output_path = out_path,
            .dynamic = true,
        };

        if (clang_compile(app->tmp, opts)) {
            hot_load(app->hot, out_path);
        }
    }

    hot_update(app->hot);
    return true;
}

static bool build_build(App *app, Cli *cli) {
    bool build = cli_flag(cli, "build", "Build an executable");
    bool watch = cli_flag(cli, "watch", "Build an executable and watch changes");
    if (!build && !watch) return false;
    if (watch && !app->changed) return true;

    Clang_Options opts = {};
    if (!build_read_opts(cli, &opts)) {
        cli_show_usage(cli, G->fmt);
        os_exit(1);
    }

    bool ret = clang_compile(app->mem, opts);
    if (build) os_exit(ret ? 0 : 1);
    return true;
}

static bool build_serve(App *app, Cli *cli) {
    if (!cli_flag(cli, "serve", "Start a simple local python http server for testing wasm builds")) return false;
    assert(os_system(S("cd out && python -m http.server")), "Failed to start python http server. Is python installed?");
    os_exit(0);
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
    if (!clang_compile(app->mem, opts)) os_exit(1);

    opts.platform = Platform_Windows;
    opts.output_path = "out/release/quest_for_nothing.exe";
    if (!clang_compile(app->mem, opts)) os_exit(1);

    opts.platform = Platform_Wasm;
    opts.output_path = "out/release/quest_for_nothing.wasm";
    if (!clang_compile(app->mem, opts)) os_exit(1);

    // Download SDL3
    if (!fs_exists(S("out/SDL3.dll"))) {
        os_system(S("curl -L -o out/SDL3.zip https://github.com/libsdl-org/SDL/releases/download/release-3.2.16/SDL3-3.2.16-win32-x64.zip"));
        os_system(S("cd out/ && unzip SDL3.zip SDL3.dll"));
    }
    os_system(S("cp out/SDL3.dll out/release/SDL3.dll"));
    os_system(S("cp src/lib/*.js out/release/"));
    os_system(S("cp src/qfn/os_wasm.html out/release/index.html"));

    if (upload) {
        os_system(S("butler push out/release tsmeets/quest-for-nothing:release --userversion $(date +'%F')"));
        os_system(S("butler push out/release tsmeets/quest-for-nothing:release-web --userversion $(date +'%F')"));
        os_system(S("rclone copy out/release fastmail:tsmeets.fastmail.com/files/tsmeets.nl/qfn/"));
    }
    os_exit(0);
    return true;
}

static void build_init(App *app, Cli *cli) {
    if (build_run(app, cli)) {
    } else if (build_format(app, cli)) {
    } else if (build_build(app, cli)) {
    } else if (build_include_graph(app, cli)) {
    } else if (build_serve(app, cli)) {
    } else if (build_all(app, cli)) {
    } else {
        cli_show_usage(cli, G->fmt);
        os_exit(1);
    }

    // We keep going, so init watch
    if (!app->watch.count) {
        watch_add(&app->watch, "src");
    }
    app->changed = watch_check(&app->watch);
}

static void os_main(void) {
    App *app = G->app;
    os_set_update_time(G->os, 100 * 1000);
    if (!app) {
        Memory *mem = mem_new();
        app = G->app = mem_struct(mem, App);
        app->mem = mem;
        app->changed = true;
    }
    app->tmp = mem_new();

    Cli cli = cli_new(G->os->argc, G->os->argv);
    build_init(app, &cli);
    // if (watch_check(&app->watch) || app->first) {
    //     app->first = false;
    //     if (app->do_build) clang_compile(app->tmp, app->build_opts);
    // }

    mem_free(app->tmp);
}
