// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// os_linux.h - Hello world testbed to show off hot reloading
#include "os_generic.h"
#include "fmt.h"

struct hello {
    mem mem;
    u32 counter;
    u64 start_time;
    u64 time;
};

void *main_init(int argc, char *argv[]) {
    os_printf("main_init called\n");

    mem m = {};
    hello *app = mem_struct(&m, hello);
    app->mem = m;

    app->start_time = os_time();
    app->time = app->start_time;
    return app;
}

void main_update(void *handle) {
    hello *app = handle;
    os_printf("main_update called: %u, t = %7d\n", app->counter, os_time() - app->start_time);
    app->counter++;

    // sleep
    app->time += 1000*1000 / 1;
    os_sleep_until(app->time);
}
