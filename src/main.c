// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// main.c - Quest For Nothing main entry point
#include "engine.h"
#include "fmt.h"
#include "game.h"
#include "gfx.h"
#include "input.h"
#include "math.h"
#include "player.h"
#include "time.h"

typedef struct {
    Memory *mem;

    // Entire game state
    Game *game;

    // Game engine, audio, video, input, timing
    Engine *eng;

    f32 cutoff;
    f32 duty;

    u32 reverb_ix;
    v2 reverb[1024 * 4];

    Image *cursor;
} App;

static App *app_init(OS *os) {
    Memory *mem = mem_new();
    App *app = mem_struct(mem, App);

    app->mem = mem;
    app->game = game_new();
    app->eng = engine_new(mem, os, "Quest For Nothing");
    app->cursor = gen_cursor(mem);
    audio_play(app->eng->audio, 0, 1e9, rand_f32(&app->game->rng));
    return app;
}

static v2 sound_shift(f32 input, f32 shift) {
    return (v2){input * (1 + shift), input * (1 - shift)};
}
static void os_audio_callback(OS *os, f32 dt, u32 count, v2 *output) {
    App *app = os->app;
    if (!app) return;

    Audio *audio = app->eng->audio;
    for (u32 i = 0; i < count; ++i) {
        v2 out = 0;

        for (u32 j = 0; j < array_count(audio->sounds); ++j) {
            Sound *snd = audio->sounds + j;
            if (!snd->id) continue;
            sound_begin_sample(snd, dt);

            // First kind, background
            if (snd->kind == 0) {
                f32 background = sound_noise_white(snd) * (1 + 0.5 * sound_sine(snd, 0.025));
                background = sound_filter(snd, 120 * (2 + sound_sine(snd, 0.001) * sound_sine(snd, 0.02) * 0.5), background).low_pass;
                out += sound_shift(background * 1.0, sound_sine(snd, 0.1) * 0.4);
            }

            if (snd->kind == 1) {
                f32 v = sound_sine(snd, 800 * snd->time * snd->pitch * (1 + sound_sine(snd, 20) * 0.5)) * f_min(1, snd->time);
                out += sound_shift(v, sound_sine(snd, 100) * 0.3);
            }

            if (snd->kind == 2) {
                // out += sound_sine(snd, 80) * f_max(f_min(app->shoot_time * 2 - .5, 1), 0) +
                //        sound_noise_freq(snd, 160 * 4 * 8, 0.5) * f_min(app->shoot_time * app->shoot_time * 2, 1);
            }

            // Sound is finished
            snd->time -= dt;
            if (snd->time < 0) snd->id = 0;
        }

        // out.y = out.x;
        out *= 0.5;

        u32 ix = app->reverb_ix++;
        if (app->reverb_ix >= array_count(app->reverb)) app->reverb_ix = 0;
        out += app->reverb[ix] * 0.3;

        f32 amp = 1.0;
        app->reverb[ix] = 0;
        for (u32 o = 0; o < 6; ++o) {
            i32 ox = (i32)ix - (i32)o;
            if (ox < 0) ox += array_count(app->reverb);
            app->reverb[ox] += out * amp;
            amp *= 0.4;
        }

        output[i] = out;
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
    App *app = os->app;
    Engine *eng = app->eng;

    engine_begin(eng);
    f32 dt = eng->dt;
    Input *input = eng->input;

    // Handle System keys (Quittng, Mouse grab, etc...)
    handle_basic_input(input, eng);

    // Player update
    Player *pl = app->game->player;
    {
        m4 mtx = m4_id();
        m4_scale(&mtx, (v3){32, 32, 1});
        if (!input->mouse_is_grabbed) m4_translate(&mtx, (v3){input->mouse_pos.x, input->mouse_pos.y, 0});
        gfx_quad_ui(eng->gfx, mtx, app->cursor);
    }

    game_update(app->game, eng);

    if (key_click(input, KEY_SPACE)) {
        audio_play(eng->audio, 1, 0.5, rand_f32(&app->game->rng) * 0.1 + 1.0);
    }

    eng->camera = pl->head_mtx;
    engine_end(app->eng);
}
