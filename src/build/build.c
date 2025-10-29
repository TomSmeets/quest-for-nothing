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

static void os_main(void) {
    if (!G->app) {
        G->app = mem_struct(G->mem, App);
        G->app->build = build_new();
    }

    Cli *cli = cli_new();
    build_update(G->app->build, cli);
}
