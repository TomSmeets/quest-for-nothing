// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// main.c - Quest For Nothing main entry point
#include "lib/global.h"
#include "lib/math.h"
#include "lib/os_main.h"
#include "qfn/cursor.h"
#include "qfn/engine.h"
#include "qfn/game.h"
#include "qfn/gfx.h"
#include "qfn/gfx_impl.h"
#include "qfn/input.h"

struct App {
    // Permanent Game memory (allocate only)
    Memory *mem;

    // Game engine, audio, video, input, timing
    Engine *eng;

    // Entire game state
    Game *game;

    // Mouse cursor
    Cursor cursor;
};

static App *app_init(void) {
    Memory *mem = mem_new();
    App *app = mem_struct(mem, App);

    app->mem = mem;
    app->eng = engine_new(mem, G->os, *G->rand, "Quest For Nothing");
    app->game = game_new(&app->eng->rng);
    cursor_load(&app->cursor, mem);
    return app;
}

static void gfx_audio_callback(u32 sample_count, v2 *samples) {
    App *app = G->app;
    if (!app) return;

    Audio *audio = &app->game->audio;

    // TODO: to cleanup we could reduce dependencies between sound, audio and music
    // TODO: Split The synth from the game
    for (u32 i = 0; i < sample_count; ++i) {
        samples[i] = sound_clip2(audio_sample(audio));
    }
}

static void handle_basic_input(App *app, Input *input, Engine *eng) {
    // Quit
    if (input->quit || (key_down(input, KEY_SHIFT) && key_down(input, KEY_Q))) {
        os_exit(0);
    }

    // Toggle fullscreen
    if (key_click(input, KEY_F)) {
        gfx_set_fullscreen(eng->gfx, !input->is_fullscreen);
    }

    // Reload level with 'R'
    if (key_click(input, KEY_R)) {
        Memory *old = app->game->mem;
        app->game = game_new(&eng->rng);
        mem_free(old);
    }
}

static void os_main(void) {
    // Initialize App
    if (!G->app) G->app = app_init();

    // Update Loop
    App *app = G->app;
    Engine *eng = app->eng;

    engine_begin(eng);

    handle_basic_input(app, eng->input, eng);
    cursor_draw(&app->cursor, eng);
    game_update(app->game, eng);

    if (app->game->debug == DBG_Texture) debug_draw_texture(eng);

    engine_end(app->eng, app->game->camera.mtx);
}
