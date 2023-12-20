// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// quest_for_nothing.c - Main entry point for "Quest For Nothing"
#include "color.h"
#include "fmt.h"
#include "gl.h"
#include "inc.h"
#include "input.h"
#include "math.h"
#include "mem.h"
#include "os_generic.h"
#include "sdl.h"
#include "str.h"
#include "vec.h"
#include "mat.h"

static void input_key_dbg(input *in) {
    mem m = {};
    for (u32 i = 0; i < array_count(in->key); ++i) {
        input_key *key = in->key + i;
        if (!key->code) break;
        if (!key->click) continue;
        os_print(fmt(&m, "key=%s\n", input_key_to_str[key->code]));
    }
    mem_clear(&m);
}

static void audio_callback(f32 dt, u32 count, v2 *buffer) {
    static f32 t0 = 0;
    static f32 t1 = 0;
    static f32 t2 = 0;

    for (u32 i = 0; i < count; ++i) {
        f32 v = f_sin(t0 * R4) * f_sin(t1 * R4);

        // compression
        // v = f_clamp(v*(1 + 8*(.5 + .5*f_sin(t2*R4))), -1, 1);

        buffer[i] = (v2){v, v};

        t0 = f_fract(t0 + dt * 440);
        t1 = f_fract(t1 + dt * 1);
        t2 = f_fract(t2 + dt * .5);
    }
}

struct App {
    mem tmp;
    mem perm;
    u32 counter;

    sdl_win *window;

    u64 time;
    u64 start_time;

    gl_t *gl;
};

// You can choose how to run this app
// - dynamically: use ./hot main.so
// - directly:    use ./main
void *main_init(int argc, char **argv) {
    os_printf("main_init called :)\n");
    mem m = {};
    App *app = mem_struct(&m, App);
    app->perm = m;
    app->window = sdl_new(&m, "Hello World");
    app->start_time = os_time();
    app->time = app->start_time;
    app->gl   = gl_init(&m, app->window->gl);
    return app;
}

void main_update(void *handle) {
    App *app = handle;
    mem *m = &app->tmp;

    sdl_win *win = app->window;
    gl_api  *gl  = win->gl;
    u32 rate = 200;

    // os_printf("time:    %f\n", (app->time - app->start_time) / 1e6);
    // os_printf("Counter: %u\n", app->counter++);
    sdl_begin(win);

    gl->glClearColor(.5, .5, .5, 1);
    gl->glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    input_key_dbg(&win->input);
    if (win->input.quit || input_is_down(&win->input, KEY_Q)) {
        sdl_quit(win);
        os_exit(0);
    }

    sdl_end(win);

    // The best way to do timing
    // 1. compute target future time: t1 = t0 + dt
    // 2. sleep until we reach that time
    // if we miss our mark a little we can correct it again the next frame
    app->time += 1000 * 1000 / rate;
    os_sleep_until(app->time);
}
