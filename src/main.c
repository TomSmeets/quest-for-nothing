// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// main.c - Quest For Nothing main entry point
#include "audio.h"
#include "fmt.h"
#include "game.h"
#include "input.h"
#include "math.h"
#include "ogl.h"
#include "os.h"
#include "player.h"
#include "sdl.h"

typedef struct {
    Game *game;
    u64 time;
    u64 delay;
    f32 dt;

    Sdl *sdl;
    OGL *gl;

    Audio audio;

    f32 cutoff;
    f32 duty;
    Memory *mem;

    f32 shoot_time;
} App;

static void app_set_fps(App *app, u32 rate) {
    app->delay = 1000000 / (u64)rate;
    app->dt = 1.0f / (f32)rate;
}

static App *app_init(void) {
    Memory *mem = mem_new();
    App *app = mem_struct(mem, App);

    app->mem = mem;
    app->game = game_new();

    app->time = os_time();
    app->time /= 1000;
    app->time /= 1000;
    app->time *= 1000;
    app->time *= 1000;
    app_set_fps(app, 200);

    app->sdl = sdl_load(mem, "Quest For Nothing");
    app->gl = ogl_load(mem, app->sdl->api.SDL_GL_GetProcAddress);
    return app;
}

static void sdl_audio_callback(OS *os, f32 dt, u32 count, v2 *output) {
    App *app = os->app;
    if (!app) return;

    Input *input = &app->sdl->input;
    Audio *audio = &app->audio;

    for (u32 i = 0; i < count; ++i) {
        audio_begin_sample(audio, dt);

        f32 shoot_freq = 0;
        f32 shoot_volume = 0;

        if (0 && app->shoot_time < 1) {
            shoot_volume = 1 - app->shoot_time;
            shoot_volume *= shoot_volume;
            shoot_freq = (1 - app->shoot_time) * 200 + 400;
            app->shoot_time += dt * 2;
        }

        f32 wave = audio_sine(audio, shoot_freq * (1.0 - 0.1 * audio_sine(audio, 20)));

        v2 sample = {};
        sample.x = (wave + audio_sine(audio, 40) * 0.1) * shoot_volume;
        sample.y = (wave + audio_sine(audio, 45) * 0.1) * shoot_volume;
        output[i] = sample;
    }
}

// Sleep until next frame
static void app_sleep(App *app) {
    u64 time = os_time();
    // os_printf("t = %u us r = %u fps\n", time - app->time, 1000000 / (time - app->time));

    // We are a frame ahead
    if (app->time > time + app->delay) {
        os_printf("We are ahead\n");
        app->time = time;
    }

    // We are a frame behind
    if (app->time + app->delay < time) {
        os_printf("We are behind\n");
        app->time = time;
    }

    // Compute next frame time
    app->time += app->delay;

    // sleep
    if (app->time > time) {
        os_sleep(app->time - time);
    }
}

static void os_main(OS *os) {
    if (!os->app) {
        os->app = app_init();
    }

    App *app = os->app;
    Memory *tmp = mem_new();

    if (os->reloaded) {
        // Reload
        app->gl = ogl_load(app->mem, app->sdl->api.SDL_GL_GetProcAddress);

        // Regen game
        game_free(app->game);
        app->game = game_new();
    }

    // Handle Input
    Input *input = sdl_poll(app->sdl);
    if (input->quit || key_down(input, KEY_Q)) os_exit(0);

    // Handle Mouse Grabbing
    if (0 && key_click(input, KEY_MOUSE_LEFT)) {
        sdl_set_mouse_grab(app->sdl, true);
    }

    if (input->focus_lost || key_click(input, KEY_ESCAPE)) {
        sdl_set_mouse_grab(app->sdl, false);
    }

    if (key_click(input, KEY_G)) {
        sdl_set_mouse_grab(app->sdl, !input->mouse_is_grabbed);
    }

    Player *pl = app->game->player;
    Player_Input in = player_parse_input(input);
    player_update(pl, app->dt, &in);

    // Render
    m4 player_mtx = m4_id();
    m4_rot_z(&player_mtx, pl->rot.z * PI); // Roll
    m4_rot_x(&player_mtx, pl->rot.x * PI); // Pitch
    m4_rot_y(&player_mtx, pl->rot.y * PI); // Yaw
    m4_trans(&player_mtx, pl->pos);
    m4_trans(&player_mtx, (v3){0, .5, 0});

    m4 proj = m4_id();
    m4_mul_inv(&proj, &player_mtx);
    m4_perspective_to_clip(&proj, 70, (f32)input->window_size.x / (f32)input->window_size.y, 0.1, 32.0);

    ogl_begin(app->gl);

    for (Monster *mon = app->game->monsters; mon; mon = mon->next) {
        monster_update(mon, app->dt, &app->game->rng);
        ogl_sprite(app->gl, pl->pos, mon->pos, mon->image);
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
        if (cell->y_neg) ogl_quad(app->gl, &mtx, cell->y_neg);
    }

    if (key_click(input, KEY_MOUSE_LEFT)) app->shoot_time = 0;

    ogl_draw(app->gl, &proj.fwd, input->window_size);
    // os_printf("%v3f\n", pl->pos);

    // Finish
    sdl_swap_window(app->sdl);
    mem_free(tmp);
    app_sleep(app);
}
