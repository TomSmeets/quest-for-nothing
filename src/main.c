// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// main.c - Quest For Nothing main entry point
#include "engine.h"
#include "game.h"
#include "gfx.h"
#include "gfx_impl.h"
#include "global.h"
#include "input.h"
#include "math.h"
#include "os.h"
#include "os_impl.h"

typedef struct App App;
struct App {
    Memory *mem;

    // Entire game state
    Game *game;

    // Game engine, audio, video, input, timing
    Engine *eng;

    Image *cursor;
};

static App *app_init(void) {
    Memory *mem = mem_new();
    App *app = mem_struct(mem, App);

    app->mem = mem;
    app->eng = engine_new(mem, G->os, *G->rand, "Quest For Nothing");
    app->game = game_new(&app->eng->rng);
    app->cursor = gen_cursor(mem);
    return app;
}

static void gfx_audio_callback(u32 sample_count, v2 *samples) {
    App *app = G->app;
    if (!app) return;

    for (u32 i = 0; i < sample_count; ++i) {
        f32 sample = game_audio(app->game, app->eng);
        sample *= 0.25;
        sample = f_clamp(sample, -1, 1);
        samples[i] = (v2){sample, sample};
    }
}

static void handle_basic_input(App *app, Input *input, Engine *eng) {
    // Quit
    if (input->quit || (key_down(input, KEY_SHIFT) && key_down(input, KEY_Q))) {
        os_exit(0);
    }

    // Capture Mouse
    if (key_click(input, KEY_MOUSE_LEFT)) {
        gfx_set_grab(eng->gfx, true);
    }

    // Release Grab on focus lost or Esc
    if ((input->focus_lost || key_click(input, KEY_ESCAPE)) && input->mouse_is_grabbed) {
        gfx_set_grab(eng->gfx, false);
    }

    // Grab with G
    if (key_click(input, KEY_G)) {
        gfx_set_grab(eng->gfx, !input->mouse_is_grabbed);
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

static void draw_cursor(App *app) {
    Engine *eng = app->eng;
    Image *cursor = app->cursor;
    Input *input = eng->input;
    Gfx *gfx = eng->gfx;

    m4 mtx = m4_id();
    m4_image_ui(&mtx, cursor);
    if (!input->mouse_is_grabbed) {
        m4_translate(&mtx, (v3){input->mouse_pos.x, input->mouse_pos.y, 0});
    }
    gfx_quad_ui(gfx, mtx, cursor);
}

static void os_main(void) {
    // Initialize App
    if (!G->app) G->app = app_init();

    // Update Loop
    App *app = G->app;
    Engine *eng = app->eng;

    engine_begin(eng);

    handle_basic_input(app, eng->input, eng);
    draw_cursor(app);
    game_update(app->game, eng);

    if (app->game->debug == DBG_Texture) {
        *gfx_pass_quad(eng->gfx, &eng->gfx->pass_ui) = (Gfx_Quad){
            .x = {400, 0, 0},
            .y = {0, 400, 0},
            .z = {0, 0, 1},
            .w = {0, 0, 0},
            .uv_pos = {0, 0},
            .uv_size = {1, 1},
        };

        // Text
        m4 mtx = m4_id();
        m4_translate_x(&mtx, -eng->input->window_size.x * .5 + 3 * 4);
        m4_translate_y(&mtx, eng->input->window_size.y * .5 - 3 * 4);
        gfx_quad_ui(eng->gfx, mtx, eng->ui->image);
        ui_text(eng->ui, mtx, "Hello World!\n0123456789\n3.141592");
    }

    engine_end(app->eng, app->game->camera.mtx);
}
