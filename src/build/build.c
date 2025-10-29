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
    App *app = G->app = mem_struct(G->mem, App);
    app->build = build_new();
    return app;
}

static void os_main(void) {
    App *app = app_load();
    Cli *cli = cli_new();
    build_update(app->build, cli);
}
