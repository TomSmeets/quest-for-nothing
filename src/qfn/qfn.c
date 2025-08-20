// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// main.c - Quest For Nothing main entry point
#include "gfx/gfx.h"
#include "gfx/gfx_impl.h"
#include "gfx/input.h"
#include "lib/global.h"
#include "lib/math.h"
#include "lib/os_main.h"
#include "qfn/cursor.h"
#include "qfn/engine.h"
#include "qfn/game.h"

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

static App *AUDIO_CALLBACK_STATE;

static App *app_load(void) {
    // Already loaded
    if (G->app) {
        AUDIO_CALLBACK_STATE = G->app;
        return G->app;
    }

    Memory *mem = mem_new();
    App *app = mem_struct(mem, App);
    app->mem = mem;
    app->eng = engine_new(mem, *G->rand, "Quest For Nothing");
    app->game = game_new(&app->eng->rng);
    AUDIO_CALLBACK_STATE = app;
    G->app = app;
    cursor_load(&app->cursor, mem);
    return app;
}

static void gfx_audio_callback(u32 sample_count, v2 *samples) {
    App *app = AUDIO_CALLBACK_STATE;
    Audio *audio = &app->game->audio;
    mutex_lock(&audio->mutex);
    for (u32 i = 0; i < sample_count; ++i) {
        samples[i] = sound_clip2(audio_sample(audio));
    }
    mutex_unlock(&audio->mutex);
}

static void os_main(void) {
    // Initialize App
    App *app = app_load();

    // Update Loop
    Game *game = app->game;
    Engine *eng = app->eng;

    engine_begin(eng);
    Input *input = eng->input;

    // Basic input
    if (input->quit || (input_click(input, KEY_Q) && input_down(input, KEY_SHIFT))) os_exit(0);
    input_toggle(input, KEY_M, &game->audio.mute);
    input_cycle(input, KEY_4, &game->debug, DBG_COUNT);

    // Toggle fullscreen
    if (input_click(input, KEY_F)) {
        gfx_set_fullscreen(eng->gfx, !input->is_fullscreen);
    }

    // Reload level with 'R'
    if (input_click(input, KEY_R)) {
        Memory *old = app->game->mem;
        app->game = game_new(&eng->rng);
        mem_free(old);
    }

    game_update(game, eng);

    // Graphics
    cursor_draw(&app->cursor, eng);
    engine_end(app->eng, game->player->camera);
}
