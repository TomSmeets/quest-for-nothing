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
};

static App *app_load(void) {
    if (G->app) return G->app;

    Build *build = build_new();
    build_add_source(build, S("src"));

    App *app = G->app = mem_struct(G->mem, App);
    app->build = build;
    return app;
}

static void os_main(void) {
    App *app = app_load();
    Cli cli = cli_new(G->argc, G->argv);
    do {
        // Basics
        if (build_build(app->build, &cli)) break;
        if (build_run(app->build, &cli)) break;

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
