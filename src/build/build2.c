#include "clang.h"
#include "cli2.h"
#include "fmt.h"
#include "hot.h"
#include "include_graph.h"
#include "os_main.h"
#include "watch.h"

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
    bool result = os_system(S("clang-format --verbose -i */*.{h,c}"));
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

    if (!clang_compile(app->mem, opts)) os_exit(1);
    if (build) os_exit(0);
    return true;
}

static bool build_serve(App *app, Cli *cli) {
    if (!cli_flag(cli, "serve", "Start a simple local python http server for testing wasm builds")) return false;
    assert(os_system(S("cd out && python -m http.server")), "Failed to start python http server. Is python installed?");
    os_exit(0);
    return true;
}

static void build_init(App *app, Cli *cli) {
    if (build_run(app, cli)) {
    } else if (build_format(app, cli)) {
    } else if (build_build(app, cli)) {
    } else if (build_include_graph(app, cli)) {
        // } else if (cli_flag(cli, "serve", "Start a simple local python http server for testing wasm builds")) {
        //     assert(os_system("cd out && python -m http.server"), "Failed to start python http server. Is python installed?");
        //     os_exit(0);
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
