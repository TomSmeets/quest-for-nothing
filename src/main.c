// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// main.c - Quest For Nothing main entry point
#include "audio.h"
#include "fmt.h"
#include "game.h"
#include "gfx.h"
#include "input.h"
#include "math.h"
#include "player.h"
#include "time.h"

typedef struct {
    // Entire game state
    Game *game;

    // Frame timing, for running at a consistent framerate
    Time time;

    // Graphics api. Platform dependent.
    Gfx *gfx;

    Audio audio;

    f32 cutoff;
    f32 duty;
    Memory *mem;

    u32 reverb_ix;
    v2 reverb[1024 * 4];

    Image *cursor;
} App;

static App *app_init(void) {
    Memory *mem = mem_new();
    App *app = mem_struct(mem, App);

    app->mem = mem;
    app->game = game_new();
    app->gfx = gfx_init(mem, "Quest For Nothing");
    app->cursor = gen_cursor(mem);
    audio_play(&app->audio, 0, 1e9, rand_f32(&app->game->rng));
    return app;
}

static v2 sound_shift(f32 input, f32 shift) {
    return (v2){input * (1 + shift), input * (1 - shift)};
}
static void os_audio_callback(OS *os, f32 dt, u32 count, v2 *output) {
    App *app = os->app;
    if (!app) return;

    Audio *audio = &app->audio;
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
                out += sound_shift(background * 0.2, sound_sine(snd, 0.1) * 0.4);
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

static void handle_basic_input(Input *input, Gfx *gfx) {
    // Quit
    if (input->quit || (key_down(input, KEY_SHIFT) && key_down(input, KEY_Q))) {
        os_exit(0);
    }

    if (key_click(input, KEY_MOUSE_LEFT)) {
        gfx_set_grab(gfx, true);
    }

    // Release Grab on focus lost or Esc
    if ((input->focus_lost || key_click(input, KEY_ESCAPE)) && input->mouse_is_grabbed) {
        fmt_s(OS_FMT, "RELEASE\n");
        gfx_set_grab(gfx, false);
    }

    // Grab with G
    if (key_click(input, KEY_G)) {
        fmt_s(OS_FMT, "Grab!\n");
        gfx_set_grab(gfx, !input->mouse_is_grabbed);
    }

    if (key_click(input, KEY_F)) {
        gfx_set_fullscreen(gfx, !input->is_fullscreen);
    }
}

static void os_main(OS *os) {
    // Initialize App
    if (!os->app) os->app = app_init();
    App *app = os->app;

    // Allocate memory for this frame (and free at the end of the frame)
    // These memory blocks are reused every frame, so this is very cheap
    Memory *tmp = mem_new();

    // Frame Timing
    // 'dt' is the time this frame will take in secods
    f32 dt = time_begin(&app->time, 120);

    // Read Input and start render
    Input *input = gfx_begin(app->gfx);

    if (0) {
        fmt_s(OS_FMT, "P: ");
        fmt_v3(OS_FMT, app->game->player->pos);
        fmt_s(OS_FMT, "\n");

        fmt_s(OS_FMT, "F: ");
        fmt_v3(OS_FMT, app->game->player->head_mtx.z);
        fmt_s(OS_FMT, "\n");
    }

    // Handle System keys (Quittng, Mouse grab, etc...)
    handle_basic_input(input, app->gfx);

    // Player update
    Player *pl = app->game->player;
    {
        m4 mtx = m4_id();
        m4_scale(&mtx, (v3){32, 32, 1});
        if (!input->mouse_is_grabbed) m4_translate(&mtx, (v3){input->mouse_pos.x, input->mouse_pos.y, 0});
        gfx_quad_ui(app->gfx, mtx, app->cursor);
    }

    game_update(app->game, &app->audio, app->gfx, input, dt);

    if (key_click(input, KEY_SPACE)) {
        audio_play(&app->audio, 1, 0.5, rand_f32(&app->game->rng) * 0.1 + 1.0);
    }

    // Finish
    gfx_end(app->gfx, pl->head_mtx);
    mem_free(tmp);
    os->sleep_time = time_end(&app->time);
}
