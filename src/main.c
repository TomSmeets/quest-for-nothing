// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// main.c - Quest For Nothing main entry point
#include "fmt.h"
#include "game.h"
#include "os.h"
#include "input.h"
#include "math.h"
#include "sdl.h"

typedef struct {
    Game *game;
    u64 time;
    Sdl *sdl;

    u32 wave_ix;
    f32 wave[64];
} App;

static App *app_init(void) {
    Memory *mem = mem_new();
    App *app = mem_struct(mem, App);
    app->game = game_new();
    app->time = os_time();
    app->sdl = sdl_load(mem, "Quest For Nothing");
    return app;
}

static f32 *audio_var(App *app) {
    if (app->wave_ix == array_count(app->wave)) return 0;
    u32 ix = app->wave_ix++;
    return app->wave + ix;
}

static f32 audio_wave(App *app, f32 dt) {
    f32 *v = audio_var(app);
    if(!v) return 0;

    f32 ret = *v;
    *v += dt;
    *v -= (i32) *v;
    return ret;
}

static f32 audio_filter(App *app, f32 dt, f32 sample) {
    f32 *v = audio_var(app);
    if(!v) return 0;
    f32 a = dt*100;
    // TODO: Continue!
    *v = (*v)*(1-a) + sample*a;
    return *v;
}

static void sdl_audio_callback(OS *os, f32 dt, u32 count, v2 *output) {
    App *app = os->app;

    Input *input = &app->sdl->input;

    f32 mx = (f32) app->sdl->input.mouse_pos.x / (f32) app->sdl->input.window_size.x;
    f32 my = 1-(f32) app->sdl->input.mouse_pos.y / (f32) app->sdl->input.window_size.y;
    
    for(u32 i = 0; i < count; ++i) {
        app->wave_ix = 0;

        f32 v = 0.0;
        v += (audio_wave(app, dt*110*(1+my)) > mx ? 1 : 0)*key_down(input, KEY_MOUSE_LEFT);
        // v = audio_filter(app, dt, v);

        f32 pan = f_sin2pi(audio_wave(app, dt*8))*.2f;
        output[i].x = v*(1 + pan);
        output[i].y = v*(1 - pan);
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

    // Handle Input
    Input *input = sdl_poll(app->sdl);
    if (input->quit) os_exit(0);

    if (input->window_resized) {
        os_printf("Resize: %4i %4i\n", input->window_size.x, input->window_size.y);
    }

    if (input->mouse_moved) {
        os_printf("Mouse:  '%-6i' '%6i'\n", input->mouse_pos.x, input->mouse_pos.y);
        os_printf("Rel:    '%+6i' '%+6i'\n", input->mouse_rel.x, input->mouse_rel.y);
        os_printf("p: '%+8.4f'\n", (float)input->mouse_pos.x / (float)input->window_size.x);
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

    // Finish
    sdl_swap_window(app->sdl);
    app_sleep(app);
}
