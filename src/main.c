// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// main.c - Quest For Nothing main entry point
#include "game.h"
#include "os.h"
#include "sdl.h"

extern int printf(const char *restrict __format, ...);

typedef struct {
    Game *game;
    u64 time;
    Sdl *sdl;
} App;

static App *app_init(void) {
    Memory *mem = mem_new();
    App *app = mem_struct(mem, App);
    app->game = game_new();
    app->time = os_time();
    app->sdl = sdl_load(mem, "Quest For Nothing");
    return app;
}

// Sleep until next frame
static void app_sleep(App *app) {
    u64 time = os_time();
    while (app->time < time) {
        app->time += 1000 * 1000;
    }
    os_sleep(app->time - time);
}

static void os_main(OS *os) {
    if (!os->app) {
        os->app = app_init();
    }

    App *app = os->app;

    // Handle Input
    Input *input = sdl_poll(app->sdl);
    if (input->quit) os_exit(0);

    printf("\n");
    printf("update\n");

    if (input->window_resized) {
        printf("Resize: %2u x %2u\n", input->window_size.x, input->window_size.y);
    }

    for (u32 i = 0; i < input->key_event_count; ++i) {
        Key key = input->key_event[i];
        printf("Key: 0x%08x '%c'\n", key, key_to_char(key));
    }

    // Finish
    sdl_swap_window(app->sdl);
    app_sleep(app);
}
