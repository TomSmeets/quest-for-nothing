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
    Game *game;
    Time_Info time;

    Gfx *gfx;

    Audio audio;

    f32 cutoff;
    f32 duty;
    Memory *mem;

    f32 shoot_time;
    f32 step_volume;

    u32 reverb_ix;
    v2 reverb[1024 * 4];

    Image *cursor;
    Image *gun;
} App;

static App *app_init(void) {
    Memory *mem = mem_new();
    App *app = mem_struct(mem, App);

    app->mem = mem;
    app->game = game_new();
    app->gfx = os_gfx_init(mem, "Quest For Nothing");

    u32 size = 5;
    app->cursor = image_new(mem, (v2u){size, size});
    for (u32 i = 0; i < size; ++i) {
        v4 col = WHITE;
        image_write(app->cursor, (v2i){i, i}, col);
        image_write(app->cursor, (v2i){i, size - 1 - i}, col);
    }

    app->gun = image_new(mem, (v2u){size, size});
    image_grid(app->gun, WHITE, BLACK);
    return app;
}

static v2 audio_shift(f32 input, f32 shift) {
    return (v2){input * (1 + shift), input * (1 - shift)};
}
static void os_audio_callback(OS *os, f32 dt, u32 count, v2 *output) {
    App *app = os->app;
    if (!app) return;

    Audio *audio = &app->audio;
    for (u32 i = 0; i < count; ++i) {
        audio_begin_sample(audio, dt);

        f32 background = audio_noise_white(audio) * (1 + 0.5 * audio_sine(audio, 0.025));
        background = audio_filter(audio, 120 * (2 + audio_sine(audio, 0.001) * audio_sine(audio, 0.02) * 0.5), background).low_pass;

        f32 noise = (1 + 0.3 * audio_noise_white(audio)) * app->step_volume * f_max(audio_sine(audio, 4), 0);
        noise = audio_filter(audio, 500, noise).high_pass;
        noise = audio_filter(audio, 1000, noise).low_pass;

        v2 out = 0;
        out += audio_shift(noise, audio_sine(audio, 1) * 0.8);
        out += audio_shift(background * 0.2, audio_sine(audio, 0.1) * 0.4);

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
    if (input->quit || key_down(input, KEY_Q)) {
        os_exit(0);
    }

    if (key_click(input, KEY_MOUSE_LEFT)) {
        os_gfx_set_mouse_grab(gfx, true);
    }

    // Release Grab on focus lost or Esc
    if ((input->focus_lost || key_click(input, KEY_ESCAPE)) && input->mouse_is_grabbed) {
        fmt_s(OS_FMT, "RELEASE\n");
        os_gfx_set_mouse_grab(gfx, false);
    }

    // Grab with G
    if (key_click(input, KEY_G)) {
        fmt_s(OS_FMT, "Grab!\n");
        os_gfx_set_mouse_grab(gfx, !input->mouse_is_grabbed);
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
    f32 dt = time_begin(&app->time, 60);

    // Start rendering
    os_gfx_begin(app->gfx);

    fmt_s(OS_FMT, "P: ");
    fmt_v3(OS_FMT, app->game->player->pos);
    fmt_s(OS_FMT, "\n");

    fmt_s(OS_FMT, "F: ");
    fmt_v3(OS_FMT, app->game->player->head_mtx.z);
    fmt_s(OS_FMT, "\n");

    // Read Input
    Input *input = os_gfx_poll(app->gfx);

    // Handle System keys (Quittng, Mouse grab, etc...)
    handle_basic_input(input, app->gfx);

    // Player update
    Player *pl = app->game->player;
    Player_Input in = player_parse_input(input);

    // Step sounds
    {
        f32 speed = v3_length_sq(in.move);
        if (!pl->on_ground) speed = 0;
        app->step_volume += (f_min(speed, 1) - app->step_volume) * 8 * dt;
    }

    player_update(pl, dt, &in);

    for (Monster *mon = app->game->monsters; mon; mon = mon->next) {
        monster_update(mon, dt, app->game->player, &app->game->rng, app->gfx);
    }

    {
        m4 mtx = m4_id();
        // m4_trans(&mtx, (v3){0, 0, 16});
        // m4_scale(&mtx, (v3){app->cursor->size.x, app->cursor->size.y, 1});
        m4_scale(&mtx, (v3){32, 32, 1});
        if (!input->mouse_is_grabbed)
            m4_translate(&mtx, (v3){input->mouse_pos.x - input->window_size.x * .5, input->mouse_pos.y - input->window_size.y * .5, 0});
        os_gfx_quad(app->gfx, mtx, app->cursor, true);
    }

    if (0) {
        m4 mtx = m4_id();
        m4_translate(&mtx, (v3){.2, .1, 0});
        m4_rotate_z(&mtx, R1 * 0.4 * app->shoot_time);
        m4_rotate_y(&mtx, -R1 * (.9 + 0.0 * f_sin(app->shoot_time)));
        m4_translate(&mtx, (v3){-1, -.5, 0});
        m4_translate(&mtx, (v3){0, 0, 1});
        os_gfx_quad(app->gfx, mtx, app->gun, true);
    }

    // Draw level
    for (Cell *cell = app->game->level; cell; cell = cell->next) {
        v3 x = {1, 0, 0};
        v3 y = {0, 1, 0};
        v3 z = {0, 0, 1};
        v3 p = v3i_to_v3(cell->pos);
        // if (cell->x_neg) ogl_quad(app->gl, &mtx, cell->x_neg);
        // if (cell->z_neg) ogl_quad(app->gl, &mtx, cell->z_neg);
        // if (cell->x_pos) ogl_quad(app->gl, &mtx, cell->x_pos);
        // if (cell->z_pos) ogl_quad(app->gl, &mtx, cell->z_pos);
        // if (cell->y_pos) ogl_quad(app->gl, &mtx, cell->y_pos);
        if (cell->x_neg) os_gfx_quad(app->gfx, (m4){-z, y, x, p - x * .5}, cell->x_neg, false);
        if (cell->z_neg) os_gfx_quad(app->gfx, (m4){x, y, z, p - z * .5}, cell->z_neg, false);

        if (cell->x_pos) os_gfx_quad(app->gfx, (m4){z, y, -x, p + x * .5}, cell->x_pos, false);
        if (cell->z_pos) os_gfx_quad(app->gfx, (m4){-x, y, -z, p + z * .5}, cell->z_pos, false);

        if (cell->y_neg) os_gfx_quad(app->gfx, (m4){x, -z, y, p}, cell->y_neg, false);
        if (cell->y_pos) os_gfx_quad(app->gfx, (m4){x, -z, y, p + y}, cell->y_pos, false);
    }

    // {
    //     v3 pos = m4s_mul_pos(&pl->body_mtx.fwd, (v3) { 0, 0, 0 });
    //     v3 dir = m4s_mul_dir(&pl->body_mtx.fwd, (v3) { 0, 0, 1 });

    //     // pos += dir;

    //     // m4s mtx = {
    //     //     {1, 0, 0, 0},
    //     //     {0, 0, 1, 0},
    //     //     {0, 1, 0, 0},
    //     //     {pos.x, pos.y, pos.z, 1},
    //     // };

    //     m4 mtx = m4_id();
    //     m4_rot_x(&mtx, R1);
    //     m4_mul(&mtx, &pl->body_mtx);
    //     os_gfx_quad(app->gfx, &mtx.fwd, app->gun, false);
    // }

    if (key_click(input, KEY_MOUSE_LEFT)) app->shoot_time = 1;
    if (app->shoot_time > 0) app->shoot_time -= dt * 4;

    // Finish
    os_gfx_end(app->gfx, pl->head_mtx);
    mem_free(tmp);
    os->sleep_time = time_end(&app->time);
}
