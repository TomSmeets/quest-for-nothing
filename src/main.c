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
} App;

static App *app_init(void) {
    Memory *mem = mem_new();
    App *app = mem_struct(mem, App);

    app->mem = mem;
    app->game = game_new();
    app->gfx = os_gfx_init(mem, "Quest For Nothing");
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
        background = audio_filter(audio, 150 * (2 + audio_sine(audio, 0.001) * audio_sine(audio, 0.02) * 0.5), background).low_pass;

        f32 noise = (1 + 0.3 * audio_noise_white(audio)) * app->step_volume * f_max(audio_sine(audio, 4) + 0.5, 0);
        noise = audio_filter(audio, 500, noise).high_pass;
        noise = audio_filter(audio, 1000, noise).low_pass;

        v2 out = 0;
        out += audio_shift(noise, audio_sine(audio, 1) * 0.8);
        out += audio_shift(background * 0.5, audio_sine(audio, 0.1) * 0.4);

        u32 ix = app->reverb_ix++;
        if (app->reverb_ix >= array_count(app->reverb)) app->reverb_ix = 0;

        out += app->reverb[ix] * 0.5;

        f32 amp = 1.0;
        app->reverb[ix] = 0;
        for (u32 o = 0; o < 8; ++o) {
            i32 ox = (i32)ix - (i32)o;
            if (ox < 0) ox += array_count(app->reverb);
            app->reverb[ox] += out * amp;
            amp *= 0.2;
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

    // Read Input
    Input *input = os_gfx_poll(app->gfx);

    // Handle System keys (Quittng, Mouse grab, etc...)
    handle_basic_input(input, app->gfx);

    // Player update
    Player *pl = app->game->player;
    Player_Input in = player_parse_input(input);
    f32 speed = v3_length_sq(in.move);
    if (!pl->on_ground) speed = 0;
    app->step_volume += (f_min(speed, 1) - app->step_volume) * 8 * dt;
    player_update(pl, dt, &in);

    // Monster Update
    for (Monster *mon = app->game->monsters; mon; mon = mon->next) {
        monster_update(mon, dt, app->game->player, &app->game->rng);
    }

    // Do collisions
    m4 player_mtx = m4_id();
    m4_rot_z(&player_mtx, pl->rot.z * PI); // Roll
    m4_rot_x(&player_mtx, pl->rot.x * PI); // Pitch
    m4_rot_y(&player_mtx, pl->rot.y * PI); // Yaw
    m4_trans(&player_mtx, pl->pos);
    m4_trans(&player_mtx, (v3){0, .5, 0});

    m4 proj = m4_id();
    m4_mul_inv(&proj, &player_mtx);
    m4_perspective_to_clip(&proj, 70, (f32)input->window_size.x / (f32)input->window_size.y, 0.1, 32.0);

    os_gfx_begin(app->gfx, &proj.fwd);

    for (Monster *mon = app->game->monsters; mon; mon = mon->next) {
        monster_update(mon, dt, app->game->player, &app->game->rng);
        os_gfx_sprite(app->gfx, pl->pos, mon->pos, mon->image, f_sin2pi(mon->wiggle) * f_min(mon->speed * 0.5, 0.2));
    }

    for (Cell *cell = app->game->level; cell; cell = cell->next) {
        v3 p = v3i_to_v3(cell->pos);
        m4s mtx = {
            {1, 0, 0, 0},
            {0, 0, 1, 0},
            {0, 1, 0, 0},
            {p.x, p.y, p.z, 1},
        };
        // if (cell->x_neg) ogl_quad(app->gl, &mtx, cell->x_neg);
        // if (cell->z_neg) ogl_quad(app->gl, &mtx, cell->z_neg);
        // if (cell->x_pos) ogl_quad(app->gl, &mtx, cell->x_pos);
        // if (cell->z_pos) ogl_quad(app->gl, &mtx, cell->z_pos);
        // if (cell->y_pos) ogl_quad(app->gl, &mtx, cell->y_pos);
        if (cell->y_neg) os_gfx_quad(app->gfx, &mtx, cell->y_neg);
    }

    if (key_click(input, KEY_MOUSE_LEFT)) app->shoot_time = 0;

    // Finish
    // TODO: pass matrix at the end, so don't render before gfx_end
    os_gfx_end(app->gfx);
    mem_free(tmp);
    os->sleep_time = time_end(&app->time);
}
