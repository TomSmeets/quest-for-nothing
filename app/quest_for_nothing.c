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
#include "ui.h"

struct App {
    mem tmp;
    mem perm;
    Global global;
    u32 counter;

    sdl_win *window;

    u64 dt;
    u64 time;
    u64 start_time;

    gl_t *gl;
    UI *ui;
};

// You can choose how to run this app
// - dynamically: use ./hot main.so
// - directly:    use ./main
void *main_init(int argc, char **argv) {
    mem m = {};
    App *app = mem_struct(&m, App);
    global_set(&app->global);
    app->perm = m;
    app->window = sdl_new(&m, "Hello World");
    app->start_time = os_time();
    app->time = app->start_time;
    app->gl   = gl_init(&m, app->window->gl);
    app->dt   = 1000 * 1000 / 200;
    app->ui   = mem_struct(&m, UI);
    return app;
}

void main_update(void *handle) {
    App *app = handle;
    mem *tmp = &app->tmp;

    sdl_win *win = app->window;
    UI *ui = app->ui;

    global_set(&app->global);
    sdl_begin(win);
    gl_clear(app->gl, win->input.window_size);

    // Handle quit
    if (win->input.quit || input_is_down(&win->input, KEY_Q)) {
        sdl_quit(win);
        os_exit(0);
    }

    // os_printf("%f %f\n", win->input.mouse_pos.x, win->input.mouse_pos.y);

    // Draw ui
    ui_begin(ui, &win->input, tmp);

    if(ui_button(ui, "Click Me!")) {
        os_print("Hello, World!\n");
    }

    if(ui_button(ui, "Other Button")) {
        os_print("Other Button!\n");
    }

    ui_newline(ui);
    ui_button(ui, "A");
    ui_button(ui, "B");
    ui_end(ui);


    Gfx *gfx = gfx_begin(tmp);
    gfx->mtx = m4_id();
    gfx_rect(gfx, (v2) { -1, -1 }, (v2) { 1, 1 }, 0);
    gl_draw(app->gl, gfx);
    gl_draw(app->gl, ui->gfx);

    sdl_end(win);

    // Wait for the next frame
    app->time += app->dt;
    os_sleep_until(app->time);
}
