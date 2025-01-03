// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// main.c - Quest For Nothing main entry point
#include "engine.h"
#include "fmt.h"
#include "game.h"
#include "gfx.h"
#include "input.h"
#include "math.h"
#include "os.h"
#include "player.h"
#include "time.h"
#include "ui.h"

typedef struct {
    Memory *mem;

    // Entire game state
    Game *game;

    // Game engine, audio, video, input, timing
    Engine *eng;

    Image *cursor;
} App;

static App *app_init(OS *os) {
    Memory *mem = mem_new();
    App *app = mem_struct(mem, App);

    app->mem = mem;
    app->eng = engine_new(mem, os, "Quest For Nothing");
    app->game = game_new(&app->eng->rng);
    app->cursor = gen_cursor(mem);
    return app;
}

static void os_gfx_audio_callback(u32 sample_count, v2 *samples) {
    App *app = OS_GLOBAL->app;
    if (!app) return;

    Audio *audio = app->eng->audio;

    // Init all samples to 0
    std_memzero(samples, sizeof(v2) * sample_count);

    for (u32 i_snd = 0; i_snd < array_count(audio->sounds); ++i_snd) {
        Sound *snd = audio->sounds + i_snd;
        for (u32 i = 0; i < sample_count; ++i) {
            // Skip finished sounds
            if (!snd->playing) break;

            // Add next sample
            samples[i] += sound_sample(snd);
        }
    }

    // Reduce volume and clamp (Protect my ears)
    for (u32 i = 0; i < sample_count; ++i) {
        v2 *sample = samples + i;
        sample->x = f_clamp(sample->x * 0.25, -1, 1);
        sample->y = f_clamp(sample->y * 0.25, -1, 1);
    }
}

static void handle_basic_input(Input *input, Engine *eng) {
    // Quit
    if (input->quit || (key_down(input, KEY_SHIFT) && key_down(input, KEY_Q))) {
        os_exit(0);
    }

    if (key_click(input, KEY_MOUSE_LEFT)) {
        // TODO: gfx -> input
        gfx_set_grab(eng->gfx, true);
    }

    // Release Grab on focus lost or Esc
    if ((input->focus_lost || key_click(input, KEY_ESCAPE)) && input->mouse_is_grabbed) {
        fmt_s(OS_FMT, "RELEASE\n");
        gfx_set_grab(eng->gfx, false);
    }

    // Grab with G
    if (key_click(input, KEY_G)) {
        fmt_s(OS_FMT, "Grab!\n");
        gfx_set_grab(eng->gfx, !input->mouse_is_grabbed);
    }

    if (key_click(input, KEY_F)) {
        gfx_set_fullscreen(eng->gfx, !input->is_fullscreen);
    }
}

static void os_main(OS *os) {
    // Initialize App
    if (!os->app) os->app = app_init(os);

    // Update Loop
    App *app = os->app;
    Engine *eng = app->eng;

    engine_begin(eng);

    // Reload level with 'R'
    if (key_click(eng->input, KEY_R)) {
        mem_free(app->game->mem);
        app->game = game_new(&eng->rng);
    }

    // Handle System keys (Quittng, Mouse grab, etc...)
    handle_basic_input(eng->input, eng);

    // Player update
    Player *pl = app->game->player;
    {
        m4 mtx = m4_id();
        if (!eng->input->mouse_is_grabbed) m4_translate(&mtx, (v3){eng->input->mouse_pos.x, eng->input->mouse_pos.y, 0});
        gfx_quad_ui(eng->gfx, mtx, app->cursor);
    }

    game_update(app->game, eng);

    if (key_click(eng->input, KEY_SPACE)) {
        Sound snd = {};
        snd.freq = sound_note_to_freq(-12 * 3.5 + rand_u32_range(&app->eng->rng, 0, 6));
        snd.src_a.freq = 1;
        snd.src_a.volume = 1;
        snd.src_a.attack_time = 0.0;
        snd.src_a.release_time = 1.0;

        snd.src_b.freq = 10;
        snd.src_b.volume = 10;
        snd.src_b.attack_time = 0.0;
        snd.src_b.release_time = 2.0;

        snd.src_c.freq = 1;
        snd.src_c.volume = .5;
        snd.src_c.attack_time = 0.1;
        snd.src_c.release_time = 2.0;
        audio_play(eng->audio, snd);
    }

    if (app->game->debug) {
        for (Monster *m = app->game->monsters; m; m = m->next) {
            if (!m->is_monster) continue;
            if (m->health <= 0) continue;
            m4 mtx = m4_id();
            gfx_quad_ui(eng->gfx, mtx, m->image);
            break;
        }

        *gfx_pass_quad(eng->gfx, &eng->gfx->pass_ui) = (OS_Gfx_Quad){
            .x = {400, 0, 0},
            .y = {0, 400, 0},
            .z = {0, 0, 1},
            .w = {0, 0, 0},
            .uv_pos = {0, 0},
            .uv_size = {1, 1},
        };
    }

    m4 mtx = m4_id();
    m4_translate_x(&mtx, -eng->input->window_size.x * .5 + 3 * 4);
    m4_translate_y(&mtx, eng->input->window_size.y * .5 - 3 * 4);
    // gfx_quad_ui(eng->gfx, mtx, eng->ui.image);
    // ui_text(&eng->ui, mtx, "Hello World!\n0123456789\n3.141592");
    engine_end(app->eng, app->game->camera.mtx);
}
