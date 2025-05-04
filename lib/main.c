// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// main.c - Example main for lib/
#include "fmt.h"
#include "os_main.h"

struct App {
    Memory *mem;
    u32 i;
};

static void os_main(void) {
    App *app = G->app;
    if (!app) {
        Memory *mem = mem_new();
        app = G->app = mem_struct(mem, App);
        app->mem = mem;
    }
    fmt_su(G->fmt, "Hello World: ", app->i++, "\n");
    os_set_update_time(G->os, 1000 * 1000);
}
