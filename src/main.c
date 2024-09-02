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
#include "time.h"

typedef struct {
    Game *game;
    Time_Info time;

    Sdl *sdl;
    OGL *gl;

    Audio audio;

    f32 cutoff;
    f32 duty;
    Memory *mem;

    f32 shoot_time;
} App;

static App *app_init(void) {
    Memory *mem = mem_new();
    App *app = mem_struct(mem, App);

    app->mem = mem;
    app->game = game_new();
    app->sdl = sdl_load(mem, "Quest For Nothing");
    app->gl = ogl_load(mem, app->sdl->api.SDL_GL_GetProcAddress);
    return app;
}

static void sdl_audio_callback(OS *os, f32 dt, u32 count, v2 *output) {
    App *app = os->app;
    if (!app) return;

    Audio *audio = &app->audio;
    for (u32 i = 0; i < count; ++i) {
        audio_begin_sample(audio, dt);
        output[i] = 0;
    }
}

static void handle_basic_input(Input *input, Sdl *sdl) {
    // Quit
    if (input->quit || key_down(input, KEY_Q)) {
        os_exit(0);
    }

    if (key_click(input, KEY_MOUSE_LEFT)) {
        sdl_set_mouse_grab(sdl, true);
    }

    // Release Grab on focus lost or Esc
    if (input->focus_lost || key_click(input, KEY_ESCAPE)) {
        sdl_set_mouse_grab(sdl, false);
    }

    // Grab with G
    if (key_click(input, KEY_G)) {
        sdl_set_mouse_grab(sdl, !input->mouse_is_grabbed);
    }
}

static void os_main(OS *os) {
    // Initialize App
    if (!os->app) os->app = app_init();

    App *app = os->app;

    // Reload some things for debugging
    if (os->reloaded) {
        app->gl = ogl_load(app->mem, app->sdl->api.SDL_GL_GetProcAddress);
    }

    // Allocate memory for this frame (and free at the end of the frame)
    // These memory blocks are reused every frame, so this is very cheap
    Memory *tmp = mem_new();

    // Frame Timing
    // 'dt' is the time this frame will take in secods
    f32 dt = time_begin(&app->time, 200);

    // Handle Input
    Input *input = sdl_poll(app->sdl);
    handle_basic_input(input, app->sdl);

    Player *pl = app->game->player;
    Player_Input in = player_parse_input(input);
    player_update(pl, dt, &in);

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

    ogl_begin(app->gl, &proj.fwd, input->window_size);

    for (Monster *mon = app->game->monsters; mon; mon = mon->next) {
        monster_update(mon, dt, app->game->player, &app->game->rng);
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

    ogl_draw(app->gl);

    // Finish
    sdl_swap_window(app->sdl);
    mem_free(tmp);

    os->sleep_time = time_end(&app->time);
}
