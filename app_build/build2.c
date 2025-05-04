#include "clang.h"
#include "cli2.h"
#include "fmt.h"
#include "hot.h"
#include "include_graph.h"
#include "os_main.h"
#include "watch.h"

struct App {
    Watch watch;

    // Build
    bool do_build;
    Clang_Options build_opts;

    // Hot
    bool do_hot;
    Hot *hot;
    char *hot_path;
    char *hot_output;
    u32 hot_argc;
    char **hot_argv;
    Fmt hot_output_fmt;

    // First time?
    bool first;
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

static void os_main(void) {
    Memory *tmp = mem_new();
    App *app = G->app;
    if (!app) {
        Memory *mem = mem_new();
        app = G->app = mem_struct(mem, App);

        Cli cli = cli_new(G->os->argc, G->os->argv);

        if (cli_flag(&cli, "run", "Run an application with dynamic hot reloading")) {
            char *input_path = cli_value(&cli, "<INPUT>", "Input file");
            if (!input_path) {
                cli_show_usage(&cli, G->fmt);
                os_exit(1);
            }

            app->hot_path = input_path;
            app->hot_argv = cli.argv + cli.ix - 1;
            app->hot_argc = cli.argc - cli.ix + 1;
            app->hot = hot_new(mem, app->hot_argc, app->hot_argv);
            app->do_hot = true;
        } else if (cli_flag(&cli, "format", "Run code formatter")) {
            assert(os_system("clang-format --verbose -i */*.{h,c}"), "Format failed!");
        } else if (cli_flag(&cli, "build", "Build an executable")) {
            Clang_Options opts = {};
            if (!build_read_opts(&cli, &opts)) {
                cli_show_usage(&cli, G->fmt);
                os_exit(1);
            }
            clang_compile(tmp, opts);
        } else if (cli_flag(&cli, "watch", "Build an executable and watch changes")) {
            if (!build_read_opts(&cli, &app->build_opts)) {
                cli_show_usage(&cli, G->fmt);
                os_exit(1);
            }
            app->do_build = true;
        } else if (cli_flag(&cli, "include-graph", "Generate Include graph")) {
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
            cli_show_usage(&cli, G->fmt);
            os_exit(1);
        }

        if (!app->do_build && !app->do_hot) {
            os_exit(0);
        }

        app->first = 1;

        // We keep going, so init watch
        watch_add(&app->watch, "app_build");
        watch_add(&app->watch, "src");
        watch_add(&app->watch, "lib");
    }

    if (watch_check(&app->watch) || app->first) {
        app->first = false;
        if (app->do_build) clang_compile(tmp, app->build_opts);
        if (app->do_hot) {
            if (app->hot_output) {
                fs_remove(app->hot_output);
            }

            Fmt *fmt = &app->hot_output_fmt;
            fmt->used = 0;
            fmt_su(fmt, "out/hot_", os_time(), ".so");
            char *out_path = fmt_close(fmt);
            app->hot_output = out_path;
            fmt_ss(G->fmt, "OUT: ", out_path, "\n");
            if (clang_compile(
                    tmp,
                    (Clang_Options){
                        .input_path = app->hot_path,
                        .output_path = out_path,
                        .dynamic = true,
                    }
                )) {
                hot_load(app->hot, out_path);
            };
        }
    }

    if (app->do_hot) {
        hot_update(app->hot);
    } else {
        G->os->sleep_time = 100 * 1000;
    }

    mem_free(tmp);
}
