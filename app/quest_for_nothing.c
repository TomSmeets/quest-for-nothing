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
#include "parse_qoi.h"

struct App {
    mem tmp;
    mem perm;
    Global global;
    u32 counter;

    Sdl *window;

    u64 dt;
    u64 time;
    u64 start_time;

    f32 t;

    gl_t *gl;
    UI *ui;

    image *img;

    v3 pos;
    f32 pitch;
    f32 yaw;
    m4 cam;
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
    app->img = parse_qoi(&m, os_read_file(&m, "res/space_alien.qoi"));
    app->cam = m4_id();
    return app;
}

void main_update(void *handle) {
    App *app = handle;
    mem *tmp = &app->tmp;
    f32 dt = (f32) app->dt / 1e6;

    Sdl *win = app->window;

    global_set(&app->global);
    sdl_begin(win);
    gl_clear(app->gl, win->input.window_size);

    // Handle quit
    if (win->input.quit || input_is_down(&win->input, KEY_Q)) {
        sdl_quit(win);
        os_exit(0);
    }

    m4 look = m4_id();
    m4_rot_y(&look, app->yaw);
    m4_trans(&look, app->pos);

    v3 fwd = m4_mul_dir(&app->cam.fwd, (v3){ 0, 0,  -1 });
    v3 rgt = m4_mul_dir(&app->cam.fwd, (v3){ 1, 0,  0 });
    if (input_is_down(&win->input, KEY_W)) app->pos += fwd*dt;
    if (input_is_down(&win->input, KEY_S)) app->pos -= fwd*dt;
    if (input_is_down(&win->input, KEY_A)) app->pos -= rgt*dt;
    if (input_is_down(&win->input, KEY_D)) app->pos += rgt*dt;
    if (input_is_down(&win->input, KEY_J)) app->yaw -= dt;
    if (input_is_down(&win->input, KEY_L)) app->yaw += dt;
    if (input_is_down(&win->input, KEY_I)) app->pitch -= dt;
    if (input_is_down(&win->input, KEY_K)) app->pitch += dt;

    if(app->pitch < -R1) app->pitch = -R1;
    if(app->pitch >  R1) app->pitch =  R1;

    app->cam = m4_id();
    // m4_rot_z(&app->cam, R2);
    m4_rot_x(&app->cam, -R1);
    m4_rot_x(&app->cam, app->pitch);
    m4_rot_z(&app->cam, app->yaw);
    m4_trans(&app->cam, app->pos);

    // os_printf("%f %f\n", win->input.mouse_pos.x, win->input.mouse_pos.y);
    {
        // Draw something 3d
        Gfx *gfx = gfx_begin(tmp);
        gfx->depth = 1;
        // m4_screen_to_clip(&gfx->mtx, win->input.window_size);
        // Camera at 0,0,0 looking to (0, 0, -inf)
        m4_scale(&gfx->mtx, (v3){1,1,1}*.2);
        m4_rot_x(&gfx->mtx, -R1); // make text upright
        m4_rot_z(&gfx->mtx, (f32) app->t); // rotate around z axis
        m4_trans(&gfx->mtx, (v3){0,0, 0}); // move into position
        gfx_color(gfx, (v4){1, .5, 0, 1});
        gfx_circle(gfx, (v2){-1, 0}, 1);
        gfx_text(gfx, 0, 1, 1, "The Quick Brown fox jumps\nover the lazy dog");
        // gfx_rect(gfx, (v2){0,0}, (v2){1,1});

        // model_to_clip = view_to_clip * world_to_view * model_to_world
        // model_to_world = Translate * Rotate * Scale
        m4_mul_inv(&gfx->mtx, &app->cam);
        m4_perspective_to_clip(&gfx->mtx, 45, win->input.window_size.x / win->input.window_size.y, 0.1, 20);
        gl_draw(app->gl, gfx);
    }

    {
        // Draw ui
        UI *ui = app->ui;
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
        gl_draw(app->gl, ui->gfx);
    }

    {
        // Draw a mouse cursor
        Gfx *gfx = gfx_begin(tmp);
        gfx_color(gfx, RED);
        gfx_circle(gfx, win->input.mouse_pos, 4);

        m4_screen_to_clip(&gfx->mtx, win->input.window_size);
        gl_draw(app->gl, gfx);
    }

    sdl_end(win);

    // Wait for the next frame
    app->time += app->dt;
    app->t    += app->dt / 1e6;
    os_sleep_until(app->time);
    mem_clear(&app->tmp);
}
