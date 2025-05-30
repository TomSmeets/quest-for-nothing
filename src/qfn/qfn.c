// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// main.c - Quest For Nothing main entry point
#include "cursor.h"
#include "engine.h"
#include "game.h"
#include "gfx.h"
#include "gfx_impl.h"
#include "global.h"
#include "input.h"
#include "math.h"
#include "os_main.h"

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

    // Sample audio
    //   Sound: Sound Synthesis
    //   Audio: Sequencer, enquey sounds
    //   Music: Parse or generate music and send to sequencer
    //
    // TODO: to cleanup we could reduce dependencies between sound, audio and music
    // TODO: Split The synth from the game
    for (u32 i = 0; i < sample_count; ++i) {
        // Currently only mono audio
        v2 sample = game_audio(app->game, app->eng);

        // Reduce volume and clamp to a maximum
        sample.x = f_clamp(sample.x, -1, 1);
        sample.y = f_clamp(sample.y, -1, 1);
        sample *= 0.25;

        // Convert to stereo sound
        samples[i] = sample;
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
        mem_free(app->game->mem);
        app->game = game_new(&eng->rng);
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
