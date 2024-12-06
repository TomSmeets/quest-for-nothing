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
    Time_Info time;

    // Graphics api. Platform dependent.
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

static Image *gen_gun(Memory *mem) {
    u32 size = 5;
    Image *img = image_new(mem, (v2u){size, size});
    // image_grid(img, WHITE, GRAY);
    v3 color = {0.1, 0.1, 0.1};
    image_write(img, (v2i){2, 1}, color);
    image_write(img, (v2i){3, 1}, color);
    image_write(img, (v2i){4, 1}, color);
    image_write(img, (v2i){4, 2}, color);
    return img;
}

static Image *gen_cursor(Memory *mem) {
    u32 size = 5;
    Image *img = image_new(mem, (v2u){size, size});
    for (u32 i = 0; i < size; ++i) {
        v4 col = color_alpha(WHITE, 1);
        image_write4(img, (v2i){i, i}, col);
        image_write4(img, (v2i){i, size - 1 - i}, col);
    }
    return img;
}

static App *app_init(void) {
    Memory *mem = mem_new();
    App *app = mem_struct(mem, App);

    app->mem = mem;
    app->game = game_new();
    app->gfx = gfx_init(mem, "Quest For Nothing");
    app->gun = gen_gun(mem);
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
        m4_scale(&mtx, (v3){32, 32, 1});
        if (!input->mouse_is_grabbed) m4_translate(&mtx, (v3){input->mouse_pos.x, input->mouse_pos.y, 0});
        gfx_quad_ui(app->gfx, mtx, app->cursor);
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
        if (cell->x_neg) gfx_quad_3d(app->gfx, (m4){-z, y, x, p - x * .5}, cell->x_neg);
        if (cell->z_neg) gfx_quad_3d(app->gfx, (m4){x, y, z, p - z * .5}, cell->z_neg);

        if (cell->x_pos) gfx_quad_3d(app->gfx, (m4){z, y, -x, p + x * .5}, cell->x_pos);
        if (cell->z_pos) gfx_quad_3d(app->gfx, (m4){x, y, z, p + z * .5}, cell->z_pos);

        // OK
        if (cell->y_neg) gfx_quad_3d(app->gfx, (m4){x, z, -y, p}, cell->y_neg);
        if (cell->y_pos) gfx_quad_3d(app->gfx, (m4){x, -z, y, p + y}, cell->y_pos);
    }

    {
        m4 mtx = m4_id();
        m4_translate(&mtx, (v3){-2.0f / 5.0f, 0, 0});
        m4_scale(&mtx, 0.2f);
        m4_rotate_z(&mtx, -app->shoot_time * R1 * 0.25);
        m4_translate(&mtx, (v3){app->shoot_time * 0.05, 0, 0});
        m4_rotate_y(&mtx, R1);
        m4_translate(&mtx, (v3){.17, -0.12, .2});
        m4_apply(&mtx, pl->head_mtx);
        gfx_quad_3d(app->gfx, mtx, app->gun);
    }

    if (key_click(input, KEY_MOUSE_LEFT)) {
        app->shoot_time = 1;
        audio_play(&app->audio, 1, 0.8, rand_f32(&app->game->rng) * 0.5 + 2.0);
    }

    if (key_click(input, KEY_SPACE)) {
        audio_play(&app->audio, 1, 0.5, rand_f32(&app->game->rng) * 0.1 + 1.0);
    }

    if (app->shoot_time > 0)
        app->shoot_time -= dt * 4;
    else
        app->shoot_time = 0;

    // Finish
    gfx_end(app->gfx, pl->head_mtx);
    mem_free(tmp);
    os->sleep_time = time_end(&app->time);
}
