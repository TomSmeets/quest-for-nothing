// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// quest_for_nothing.c - Main entry point for "Quest For Nothing"
#include "inc.h"
#include "color.h"
#include "fmt.h"
#include "gl.h"
#include "input.h"
#include "math.h"
#include "mem.h"
#include "os_generic.h"
#include "sdl.h"
#include "str.h"
#include "vec.h"
#include "mat.h"
#include "sound.h"
#include "gfx.h"

static void input_key_dbg(input *in) {
    mem m = {};
    for (u32 i = 0; i < array_count(in->key); ++i) {
        input_key *key = in->key + i;
        if (!key->code) break;
        if (!key->click) continue;
        os_print(fmt(&m, "key=%s\n", enum_to_str_input_key_code[key->code]));
    }
    mem_clear(&m);
}


static void audio_callback(f32 dt, u32 count, v2 *buffer) {
    static sound snd_static;
    sound *snd = &snd_static;

    for (u32 i = 0; i < count; ++i) {
        snd_begin(snd, dt, 120);

        f32 v = 0;
        v += snd_sin(snd, 110)*snd_sin(snd, 1);
        v += snd_sin(snd, 220)*snd_sin(snd, 2);
        v += snd_sin(snd, 440)*snd_sin(snd, 3);
        v = snd_compress(v, snd->beat % 4 + 1);
        buffer[i] = (v2){v, v};
    }
}

struct App {
    mem tmp;
    mem perm;
    Global global;
    u32 counter;

    sdl_win *window;

    u64 time;
    u64 start_time;

    gl_t *gl;
};

static void play(char *music) {
    char *p = music;

    while(*p) {
        while(*p == ' ') p++;
        Note n = parse_note(&p);
        printf("%f\n", n.freq);
    }
}

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
    global_set(&app->global);
    mem *m = &app->tmp;

    sdl_win *win = app->window;
    gl_api  *gl  = win->gl;
    u32 rate = 200;

    // os_printf("time:    %f\n", (app->time - app->start_time) / 1e6);
    // os_printf("Counter: %u\n", app->counter++);
    sdl_begin(win);
    // win->audio_callback = audio_callback;

    gl_draw(app->gl, win->input.window_size);

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
