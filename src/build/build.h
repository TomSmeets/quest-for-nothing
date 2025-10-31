// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// build.h - Build tool for runing and building applications
#pragma once
#include "build/clang.h"
#include "build/hot.h"
#include "build/include_graph.h"
#include "build/watch.h"
#include "lib/cli.h"
#include "lib/fmt.h"

TYPEDEF_STRUCT(Build);
struct Build {
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
    watch_add(&build->watch, "src");
    return build;
}

// Code formatter option
// Returns true if the argument is matched
static void build_format(Build *build, Cli *cli) {
    bool match = cli_command(cli, "format", "Run code formatter");
    if (!match) return;

    char *cmd = "clang-format --verbose -i -- $(find src -name '*.h' -o -name '*.c'";
    fmt_ss(G->fmt, "Running Command: ", cmd, "\n");

    bool result = os_system(str_from(cmd));
    assert(result, "Format failed!");
    os_exit(0);
}

static void build_serve(Build *build, Cli *cli) {
    bool match = cli_command(cli, "serve", "Start a simple local http server");
    if (!match) return;
    assert(os_system(S("cd out && python -m http.server")), "Failed to start python http server. Is python installed?");
    os_exit(0);
}

static void build_build(Build *app, Cli *cli) {
    bool match = cli_command(cli, "build", "Build an executable");
    char *input = cli_value(cli, "<Input>", "Source Input Path");
    char *output = cli_value(cli, "<Output>", "Executable Output Path");

    bool watch = cli_flag(cli, "--watch", "Build an executable and watch changes");
    bool release = cli_flag(cli, "--release", "Compile in Release Mode");
    bool dynamic = cli_flag(cli, "--dynamic", "Create a dynamic library");
    bool lsp = cli_flag(cli, "--lsp", "Generate a compile_commands.json");

    bool plat_linux = cli_flag(cli, "--linux", "Compile for Linux");
    bool plat_windows = cli_flag(cli, "--windows", "Compile for Windows");
    bool plat_wasm = cli_flag(cli, "--wasm", "Compile for Webassembly");
    if (!match) return;
    if (!input || !output) return;
    if (watch && !app->changed) return;

    Clang_Options opts = {};
    opts.input_path = input;
    opts.output_path = output;
    opts.release = release;
    opts.dynamic = dynamic;
    opts.platform = Platform_Linux;
    if (plat_linux) opts.platform = Platform_Linux;
    if (plat_windows) opts.platform = Platform_Windows;
    if (plat_wasm) opts.platform = Platform_Wasm;

    if (lsp) {
        fmt_s(G->fmt, "Generating compile_commands.json\n");

        // Get current directory
        // Compile_commands needs an absolute path :/
        // otherwise this was not needed
        u32 buf_size = 1024;
        char *cwd = mem_push_uninit(G->tmp, buf_size);
        assert0(linux_getcwd(cwd, buf_size) > 0);

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
        fmt_s(fmt, "\"\n");

        fmt_s(fmt, "  }\n");
        fmt_s(fmt, "]\n");
        fmt_close(fmt);
    }

    bool ret = clang_compile(opts);
    if (!watch) os_exit(ret ? 0 : 1);
}

static void build_include_graph(Build *build, Cli *cli) {
    bool active = cli_command(cli, "graph", "Generate Include graph");
    bool rank = !cli_flag(cli, "--no-rank", "Don't use custom ranking algorithm");
    if (!active) return;
    Include_Graph *graph = include_graph_new(G->tmp);
    include_graph_read_dir(graph, S("src"));
    include_graph_tred(graph);
    if (rank) include_graph_rank(graph);
    include_graph_fmt(graph, G->fmt);
    os_exit(0);
}

static String hot_fmt(void) {
    Fmt *fmt = fmt_memory(G->tmp);
    fmt_s(fmt, "out/hot_");
    fmt_u(fmt, os_time());
    fmt_s(fmt, ".so");
    return fmt_get(fmt);
}

static void build_run(Build *build, Cli *cli) {
    // Check command
    bool match = cli_command(cli, "run", "Run an application with dynamic hot reloading");
    bool release = cli_flag(cli, "--release", "Compile in Release Mode");
    char *input_path = cli_value(cli, "<Input>", "Source Input Path");
    if (!match) return;
    if (!input_path) return;

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
            .release = release,
        };

        if (clang_compile(opts)) {
            hot_load(build->hot, out_path);
        } else {
            build->hot->child_main = 0;
        }
    }

    u32 offset = 2;
    if (release) offset += 1;
    char **argv = G->argv + offset;
    u32 argc = G->argc - offset;
    hot_update(build->hot, argc, argv);
}

static void cmd_sync(Cli *cli) {
    bool match = cli_command(cli, "sync", "Sync tlib with unison");
    char *path1 = cli_value(cli, "<Root1>", "Root 1 Path");
    char *path2 = cli_value(cli, "<Root2>", "Root 2 Path");
    bool gui = cli_flag(cli, "--gui", "Use unison GUI");

    if (!match) return;
    if (!path1 || !path2) return;
    Fmt *f = fmt_memory(G->tmp);
    if (gui) {
        fmt_s(f, "unison-gui");
    } else {
        fmt_s(f, "unison");
    }
    fmt_ss(f, " '", path1, "'");
    fmt_ss(f, " '", path2, "'");
    fmt_s(f, " -path src/lib");
    fmt_s(f, " -path src/gfx");
    fmt_s(f, " -path src/build");
    fmt_s(f, " -path .gitignore");
    fmt_s(f, " -path LICENSE.txt");
    fmt_s(f, " -path .clang-format");
    String cmd = fmt_get(f);
    fmt_s(G->fmt, "Running: ");
    fmt_str(G->fmt, cmd);
    fmt_s(G->fmt, "\n");
    os_system(cmd);
    os_exit(0);
}

static void build_update(Build *build, Cli *cli) {
    // Basics
    build_build(build, cli);
    build_run(build, cli);

    // Extras
    build_format(build, cli);
    build_serve(build, cli);
    build_include_graph(build, cli);
    cmd_sync(cli);
    cli_help(cli);

    build->changed = watch_check(&build->watch);
}
