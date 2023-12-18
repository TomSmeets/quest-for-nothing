// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// os_linux.h - Hello world testbed to show off hot reloading
#include "os_linux.h"
#include "fmt.h"

struct hello {
    u32 counter;
};

hello *main_init(int argc, char *argv[]) {
    mem m = {};
    hello *app = mem_struct(&m, hello);
    os_printf("main_init called\n");
    return app;
}

u32 main_update(hello *app) {
    os_printf("main_update called: %u\n", app->counter);
    app->counter++;
    os_sleep(100000);
    return 0;
}


int main(int argc, char **argv) {
    hello *app = main_init(argc, argv);
    for(;;) {
        u32 ret = main_update(app);
        if(ret > 0) return ret - 1;
    }
}
