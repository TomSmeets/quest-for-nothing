// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
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
    audio_play(app->eng->audio, 0, 1e9, rand_f32(&app->eng->rng));
    return app;
}

static v2 sound_shift(f32 input, f32 shift) {
    return (v2){input * (1 + shift), input * (1 - shift)};
}

static v2 sound_sample_single(Sound *snd, f32 dt) {
    sound_begin_sample(snd, dt);
    v2 out = 0;

    // First kind, background
    if (snd->kind == 0) {
        f32 background = sound_noise_white(snd) * (1 + 0.5 * sound_sine(snd, 0.025));
        background = sound_filter(snd, 120 * (2 + sound_sine(snd, 0.001) * sound_sine(snd, 0.02) * 0.5), background).low_pass;
        out += sound_shift(background * 1.0, sound_sine(snd, 0.1) * 0.4);
    }

    if (snd->kind == 1) {
        f32 v = sound_sine(snd, 800 * snd->time * snd->pitch * (1 + sound_sine(snd, 20) * 0.5)) * f_min(1, snd->time);
        out += sound_shift(v, sound_sine(snd, 40) * 0.8);
    }
    return out;
}
static void sound_sample_many(Sound *snd, f32 dt, v2 *output, u32 count) {
    for (u32 i = 0; i < count; ++i) {
        if (!snd->id) break;
        output[i] += sound_sample_single(snd, dt);
        snd->time -= dt;
        if (snd->time <= 0) snd->id = 0;
    }
}

static void os_gfx_audio_callback(u32 sample_count, v2 *samples) {
    f32 dt = 1.0f / AUDIO_SAMPLE_RATE;
    App *app = OS_GLOBAL->app;
    if (!app) return;

    Audio *audio = app->eng->audio;
    std_memzero(samples, sizeof(v2) * sample_count);

    for (u32 i_snd = 0; i_snd < array_count(audio->sounds); ++i_snd) {
        Sound *snd = audio->sounds + i_snd;
        sound_sample_many(snd, dt, samples, sample_count);
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
        m4_scale(&mtx, 32 * 4);
        if (!eng->input->mouse_is_grabbed) m4_translate(&mtx, (v3){eng->input->mouse_pos.x, eng->input->mouse_pos.y, 0});
        gfx_quad_ui(eng->gfx, mtx, app->cursor);
    }

    game_update(app->game, eng);

    if (key_click(eng->input, KEY_SPACE)) {
        audio_play(eng->audio, 1, 0.5, rand_f32(&eng->rng) * 0.1 + 1.0);
    }

    engine_end(app->eng, pl->head_mtx);
}
