// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// main.c - Quest For Nothing main entry point
#include "audio.h"
#include "fmt.h"
#include "game.h"
#include "gl_api.h"
#include "input.h"
#include "math.h"
#include "os.h"
#include "sdl.h"

typedef struct {
    Game *game;
    u64 time;

    Sdl *sdl;
    Gl *gl;

    Audio audio;

    f32 cutoff;
    f32 duty;

    Memory *mem;
} App;

static App *app_init(void) {
    Memory *mem = mem_new();
    App *app = mem_struct(mem, App);
    app->mem = mem;
    app->game = game_new();
    app->time = os_time();
    app->sdl = sdl_load(mem, "Quest For Nothing");
    app->gl = gl_load(mem, app->sdl->api.SDL_GL_GetProcAddress);
    return app;
}

static void sdl_audio_callback(OS *os, f32 dt, u32 count, v2 *output) {
    App *app = os->app;
    if(!app) return;

    Input *input = &app->sdl->input;
    Audio *audio = &app->audio;

    for (u32 i = 0; i < count; ++i) {
        audio_begin_sample(audio, dt);

        f32 wave = audio_pulse(audio, app->cutoff, app->duty);

        f32 noise_l = audio_noise_white(audio); // audio_noise_freq(audio, app->cutoff, 1.0);
        f32 noise_r = audio_noise_white(audio); // audio_noise_freq(audio, app->cutoff, 1.0);

        f32 volume_1 = audio_smooth_bool(audio, 80.0f, key_down(input, KEY_MOUSE_LEFT));
        f32 volume_2 = audio_smooth_bool(audio, 80.0f, key_down(input, KEY_MOUSE_RIGHT));

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

    // We are ahead
    if (app->time > time) app->time = time;

    // Compute next frame time
    app->time += 10 * 1000;

    // We are behind, skip some frames
    if (app->time < time) app->time = time;

    // sleep
    if (app->time > time) os_sleep(app->time - time);
}

static void os_main(OS *os) {
    if (!os->app) {
        os->app = app_init();
    }

    App *app = os->app;
    if(os->reloaded) {
        app->gl = gl_load(app->mem, app->sdl->api.SDL_GL_GetProcAddress);
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
        os_printf("Cutoff: %f\n", app->cutoff);
        os_printf("Duty:   %f\n", app->duty);
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

    // Render
    gl_draw(app->gl, input->window_size);

    // Finish
    sdl_swap_window(app->sdl);
    app_sleep(app);
}
