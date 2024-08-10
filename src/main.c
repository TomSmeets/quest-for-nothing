// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// main.c - Quest For Nothing main entry point
#include "os.h"
#include "game.h"

typedef struct {
    Game *game;
    u64 time;
} App;

static App *app_init(void) {
    Memory *mem = mem_new();
    App *app = mem_struct(mem, App);
    app->game = game_new();
    app->time = os_time();
    return app;
}

static void os_main(OS *os) {
    if(!os->app) {
        os->app = app_init();
    }

    App *app = os->app;

    log_error("HI\n");

    u64 time = os_time();
    while(app->time < time) {
        app->time += 100 * 1000;
    }
    os_sleep(app->time - time);
}
