// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// os_linux.h - Hello world testbed to show off hot reloading
#include "os.h"
#include "os_windows.h"
#include "fmt.h"

struct hello {
    mem mem;
    u32 counter;
    u64 start_time;
    u64 time;
};

hello *main_init(int argc, char *argv[]) {
    os_printf("main_init called\n");

    mem m = {};
    hello *app = mem_struct(&m, hello);
    app->mem = m;

    app->start_time = os_time();
    app->time = app->start_time;
    return app;
}

u32 main_update(hello *app) {
    os_printf("main_update called: %u, t = %7d\n", app->counter, os_time() - app->start_time);
    app->counter++;

    // sleep
    app->time += 1000*1000 / 1;
    os_sleep_until(app->time);
    return 0;
}


int main(int argc, char **argv) {
    hello *app = main_init(argc, argv);
    for(;;) {
        u32 ret = main_update(app);
        if(ret > 0) return ret - 1;
    }
}
