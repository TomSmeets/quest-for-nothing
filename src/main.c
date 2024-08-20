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

        f32 wave = audio_pulse(audio, app->cutoff, app->duty);

        f32 noise_l = audio_noise_white(audio); // audio_noise_freq(audio, app->cutoff, 1.0);
        f32 noise_r = audio_noise_white(audio); // audio_noise_freq(audio, app->cutoff, 1.0);

        f32 volume_1 = audio_smooth_bool(audio, 80.0f, key_down(input, KEY_MOUSE_LEFT));
        f32 volume_2 = audio_smooth_bool(audio, 80.0f, key_down(input, KEY_MOUSE_RIGHT));
        volume_1 = 0;

        v2 sample = {};
        sample.x = wave * volume_1 + noise_l * volume_2;
        sample.y = wave * volume_1 + noise_r * volume_2;

        sample.x = audio_filter(audio, app->cutoff, sample.x).low_pass;
        sample.y = audio_filter(audio, app->cutoff, sample.y).low_pass;

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

    app_set_fps(app, 200);

    if (os->reloaded) {
        app->gl = ogl_load(app->mem, app->sdl->api.SDL_GL_GetProcAddress);
        game_free(app->game);
        app->game = game_new();
    }

    // Handle Input
    Input *input = sdl_poll(app->sdl);
    if (input->quit || key_down(input, KEY_Q)) os_exit(0);

    if (input->window_resized) {
        os_printf("Resize: %4i %4i\n", input->window_size.x, input->window_size.y);
    }

    if (input->mouse_moved) {
        // os_printf("Mouse:  %6i %6i\n", input->mouse_pos.x, input->mouse_pos.y);
        // os_printf("Rel:    %6i %+6i\n", input->mouse_rel.x, input->mouse_rel.y);
        // os_printf("p: %f\n", (float)input->mouse_pos.x / (float)input->window_size.x);

        f32 mx = (f32)app->sdl->input.mouse_pos.x / (f32)app->sdl->input.window_size.x;
        f32 my = (f32)app->sdl->input.mouse_pos.y / (f32)app->sdl->input.window_size.y;
        app->cutoff = f_pow2((mx * 2 - 1) * 2) * 440;
        app->duty = my;
    }

    if (0 && key_click(input, KEY_MOUSE_LEFT)) {
        sdl_set_mouse_grab(app->sdl, true);
    }

    if (input->focus_lost || key_click(input, KEY_ESCAPE)) {
        sdl_set_mouse_grab(app->sdl, false);
    }

    if (key_click(input, KEY_G)) {
        sdl_set_mouse_grab(app->sdl, !input->mouse_is_grabbed);
    }

    for (u32 i = 0; i < input->key_event_count; ++i) {
        Key key = input->key_event[i];
        os_printf("Key[%u]: 0x%08x '%c'\n", i, key, key_to_char(key));
    }

    Player *pl = app->game->player;

    Player_Input in = {};
    if (key_down(input, KEY_W)) in.move.z -= 1;
    if (key_down(input, KEY_S)) in.move.z += 1;
    if (key_down(input, KEY_A)) in.move.x -= 1;
    if (key_down(input, KEY_D)) in.move.x += 1;
    if (key_down(input, KEY_1)) in.look.z += 1.0f / 8;
    if (key_down(input, KEY_2)) in.look.z -= 1.0f / 8;
    if (key_down(input, KEY_SPACE)) in.jump = 1;
    if (key_down(input, KEY_SPACE)) in.move.y += 1;
    if (key_down(input, KEY_SHIFT)) in.move.y -= 1;
    if (key_click(input, KEY_F)) in.fly = 1;

    if (input->mouse_is_grabbed) {
        in.look.y -= (f32)input->mouse_rel.x / 1000.0f;
        in.look.x -= (f32)input->mouse_rel.y / 1000.0f;
    }

    player_update(pl, app->dt, &in);

    // Render
    m4 player_mtx = m4_id();
    m4_rot_z(&player_mtx, pl->rot.z * PI); // Roll
    m4_rot_x(&player_mtx, pl->rot.x * PI); // Pitch
    m4_rot_y(&player_mtx, pl->rot.y * PI); // Yaw
    m4_trans(&player_mtx, pl->pos);

    m4 proj = m4_id();
    m4_mul_inv(&proj, &player_mtx);
    m4_perspective_to_clip(&proj, 70, (f32)input->window_size.x / (f32)input->window_size.y, 0.5, 32.0);

    ogl_begin(app->gl);

    for (Monster *mon = app->game->monsters; mon; mon = mon->next) {
        ogl_quad(app->gl, 0, mon->image, mon->pos);
    }

    for (Cell *cell = app->game->level; cell; cell = cell->next) {
        v3 p = v3i_to_v3(cell->pos);
        if (cell->x_neg) ogl_quad(app->gl, 1, cell->x_neg, p + 0.5 * (v3){-1, 0, 0});
        if (cell->z_neg) ogl_quad(app->gl, 2, cell->z_neg, p + 0.5 * (v3){0, 0, -1});
        if (cell->x_pos) ogl_quad(app->gl, 3, cell->x_pos, p + 0.5 * (v3){1, 0, 0});
        if (cell->z_pos) ogl_quad(app->gl, 4, cell->z_pos, p + 0.5 * (v3){0, 0, 1});
        if (cell->y_pos) ogl_quad(app->gl, 5, cell->y_pos, p + 0.5 * (v3){0, 1, 0});
        if (cell->y_neg) ogl_quad(app->gl, 6, cell->y_neg, p + 0.5 * (v3){0, -1, 0});
    }

    ogl_draw(app->gl, &proj.fwd, pl->pos, input->window_size);
    // debug_struct(pl);

    // Finish
    sdl_swap_window(app->sdl);
    mem_free(tmp);
    app_sleep(app);
}
