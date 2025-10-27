// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// build.h - Build tool for runing and building applications
#pragma once
#include "build/clang.h"
#include "build/cli.h"
#include "build/hot.h"
#include "build/include_graph.h"
#include "build/watch.h"
#include "lib/fmt.h"

// Read command line options for build targets
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

TYPEDEF_STRUCT(Build);

struct Build {
    // Source directories
    Build_Source *sources;

    // Dit a source file change on disk?
    Watch watch;
    bool changed;

    // Hot
    Hot *hot;

    // First time?
    bool first;
};

static Build *build_new(void) {
    Build *build = mem_struct(G->mem, Build);
    build->changed = true;
    build->hot = hot_new(G->mem);
    return build;
}

static void build_add_source(Build *build, String path) {
    Build_Source *src = mem_struct(G->mem, Build_Source);
    src->path = path;
    LIST_PUSH(build->sources, src);
    watch_add(&build->watch, str_c(path));
}

// Code formatter option
// Returns true if the argument is matched
static bool build_format(Build *build, Cli *cli) {
    if (!cli_flag(cli, "format", "Run code formatter")) return false;

    Fmt *cmd = fmt_memory(G->tmp);
    fmt_s(cmd, "clang-format --verbose -i --");
    fmt_s(cmd, " $(find");
    for (Build_Source *src = build->sources; src; src = src->next) {
        fmt_str(cmd, S(" "));
        fmt_str(cmd, src->path);
    }
    fmt_s(cmd, " -name '*.h' -o -name '*.c'");
    fmt_s(cmd, ")");
    fmt_s(G->fmt, "Running Command: ");
    fmt_str(G->fmt, fmt_get(cmd));
    fmt_s(G->fmt, "\n");
    bool result = os_system(fmt_get(cmd));
    assert(result, "Format failed!");
    os_exit(0);
    return true;
}

static bool build_serve(Build *build, Cli *cli) {
    if (!cli_flag(cli, "serve", "Start a simple local python http server for testing wasm builds")) return false;
    assert(os_system(S("cd out && python -m http.server")), "Failed to start python http server. Is python installed?");
    os_exit(0);
    return true;
}

static bool build_build(Build *app, Cli *cli) {
    bool build = cli_flag(cli, "build", "Build an executable");
    bool watch = cli_flag(cli, "watch", "Build an executable and watch changes");
    if (!build && !watch) return false;
    if (watch && !app->changed) return true;

    Clang_Options opts = {};
    opts.includes = app->sources;
    if (!build_read_opts(cli, &opts)) {
        cli_show_usage(cli, G->fmt);
        os_exit(1);
    }

    bool ret = clang_compile(opts);
    if (build) os_exit(ret ? 0 : 1);
    return true;
}

static bool build_opt_clangd(Build *build, Cli *cli) {
    // Generate compile commands instead of clangd
    bool doit = cli_flag(cli, "lsp", "Generate compile_commands.json based on the build settings");
    if (!doit) return false;

    // Get current directory
    u32 buf_size = 1024;
    char *cwd = mem_push_uninit(G->tmp, buf_size);
    assert0(linux_getcwd(cwd, buf_size) > 0);

    Clang_Options opts = {};
    opts.includes = build->sources;
    if (!build_read_opts(cli, &opts)) {
        cli_show_usage(cli, G->fmt);
        os_exit(1);
    }

    Fmt *fmt = fmt_open(G->tmp, "compile_commands.json");
    fmt_s(fmt, "[\n");
    fmt_s(fmt, "  {\n");

    fmt_s(fmt, "    \"directory\":");
    fmt_s(fmt, "\"");
    fmt_s(fmt, cwd);
    fmt_s(fmt, "\",\n");

    fmt_s(fmt, "    \"command\":");
    fmt_s(fmt, "\"");
    clang_fmt(fmt, opts);
    fmt_s(fmt, "\",\n");

    fmt_s(fmt, "    \"file\":");
    fmt_s(fmt, "\"");
    fmt_s(fmt, opts.input_path);
    fmt_s(fmt, "\",\n");

    fmt_s(fmt, "  },\n");
    fmt_s(fmt, "],\n");
    fmt_close(fmt);

    os_exit(0);
    return true;
}

static void build_update(Build *build) {
    build->changed = watch_check(&build->watch);
}

static bool build_include_graph(Build *build, Cli *cli) {
    bool active = cli_flag(cli, "include-graph", "Generate Include graph");
    if (!active) return false;
    Include_Graph *graph = include_graph_new(G->tmp);
    for (Build_Source *src = build->sources; src; src = src->next) {
        include_graph_read_dir(graph, src->path);
    }
    include_graph_tred(graph);
    // include_graph_rank(graph);
    include_graph_fmt(graph, G->fmt);
    os_exit(0);
    return true;
}

static String hot_fmt(void) {
    Fmt *fmt = fmt_memory(G->tmp);
    fmt_s(fmt, "out/hot_");
    fmt_u(fmt, os_time());
    fmt_s(fmt, ".so");
    return fmt_get(fmt);
}

static bool build_run(Build *build, Cli *cli) {
    // Check command
    if (!cli_flag(cli, "run", "Run an application with dynamic hot reloading")) return false;

    char *input_path = cli_value(cli, "<INPUT>", "Input file");

    if (!input_path) {
        cli_show_usage(cli, G->fmt);
        os_exit(1);
    }

    if (build->changed) {
        // Format new output file
        String out_path = hot_fmt();
        fmt_s(G->fmt, "OUT: ");
        fmt_str(G->fmt, out_path);
        fmt_s(G->fmt, "\n");

        Clang_Options opts = {
            .input_path = input_path,
            .output_path = (char *)out_path.data,
            .dynamic = true,
            .includes = build->sources,
        };

        if (clang_compile(opts)) {
            hot_load(build->hot, out_path);
        } else {
            build->hot->child_main = 0;
        }
    }

    u32 arg_ix = cli->ix - 1;
    hot_update(build->hot, cli->argc - arg_ix, cli->argv + arg_ix);
    return true;
}
