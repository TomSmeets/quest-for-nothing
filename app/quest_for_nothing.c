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

struct Camera {
    v3 pos;
    f32 pitch;
    f32 yaw;

    m4 view_to_world;
    m4 world_to_clip;

    bool grab_mouse;
};

static void cam_update(Camera *cam, Sdl *win, f32 dt) {
    m4 look = m4_id();
    m4_rot_y(&look, cam->yaw);
    m4_trans(&look, cam->pos);

    v3 fwd = m4_mul_dir(&cam->view_to_world.fwd, (v3){ 0, 0, -1 });
    v3 rgt = m4_mul_dir(&cam->view_to_world.fwd, (v3){ 1, 0,  0 });
    if (input_is_down(&win->input, KEY_W)) cam->pos += fwd*dt;
    if (input_is_down(&win->input, KEY_S)) cam->pos -= fwd*dt;
    if (input_is_down(&win->input, KEY_A)) cam->pos -= rgt*dt;
    if (input_is_down(&win->input, KEY_D)) cam->pos += rgt*dt;
    if (input_is_down(&win->input, KEY_J)) cam->yaw -= dt;
    if (input_is_down(&win->input, KEY_L)) cam->yaw += dt;
    if (input_is_down(&win->input, KEY_I)) cam->pitch -= dt;
    if (input_is_down(&win->input, KEY_K)) cam->pitch += dt;

    if (input_is_click(&win->input, KEY_G)) {
        cam->grab_mouse = !cam->grab_mouse;
        sdl_grab_mouse(win, cam->grab_mouse);
    }

    if(cam->grab_mouse) {
        cam->yaw   += win->input.mouse_rel.x*0.002;
        cam->pitch += win->input.mouse_rel.y*0.002;
    }

    if(cam->pitch < -R1) cam->pitch = -R1;
    if(cam->pitch >  R1) cam->pitch =  R1;

    cam->view_to_world = m4_id();
    m4_rot_x(&cam->view_to_world, -R1);
    m4_rot_x(&cam->view_to_world, cam->pitch);
    m4_rot_z(&cam->view_to_world, cam->yaw);
    m4_trans(&cam->view_to_world, cam->pos);
   
    cam->world_to_clip = m4_id();
    m4_mul_inv(&cam->world_to_clip, &cam->view_to_world);
    m4_perspective_to_clip(&cam->world_to_clip, 45, win->input.window_size.x / win->input.window_size.y, 0.1, 20);
}

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
    Camera cam;
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

    // freecam movement
    cam_update(&app->cam, win, dt);

    {
        // Draw something 3d
        Gfx *gfx = gfx_begin(tmp);
        gfx->depth = 1;
        gfx->world_to_clip = app->cam.world_to_clip;

        gfx->mtx = m4_id();
        m4_scale(&gfx->mtx, (v3){1,1,1}*.2);
        m4_rot_x(&gfx->mtx, -R1); // make text upright
        m4_trans(&gfx->mtx, (v3){0,0,0}); // move into position
        gfx_color(gfx, (v4){1, .5, 0, 1});
        gfx_circle(gfx, (v2){-1, 0}, 1);
        gfx_text(gfx, 0, 1, 1, "The Quick Brown fox jumps\nover the lazy dog");

        gfx->mtx = m4_id();
        m4_scale(&gfx->mtx, (v3){1,1,1}*.2);
        m4_rot_x(&gfx->mtx, -R1); // make text upright
        m4_trans(&gfx->mtx, (v3){0,1,0}); // move into position
        gfx_color(gfx, (v4){0, .5, 0, 1});
        gfx_circle(gfx, (v2){-1, 0}, 1);
        gfx_text(gfx, 0, 1, 1, "The Quick Brown fox jumps\nover the lazy dog");

        gl_draw(app->gl, gfx);
    }

    {
        // Draw ui
        UI *ui = app->ui;
        ui_begin(ui, &win->input, tmp);
        ui->disable_interaction =  app->cam.grab_mouse;
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

    if(!app->cam.grab_mouse) {
        // Draw a mouse cursor
        Gfx *gfx = gfx_begin(tmp);
        gfx_color(gfx, RED);
        gfx_circle(gfx, win->input.mouse_pos, 4);
        m4_screen_to_clip(&gfx->world_to_clip, win->input.window_size);
        gl_draw(app->gl, gfx);
    }

    sdl_end(win);

    // Wait for the next frame
    app->time += app->dt;
    app->t    += app->dt / 1e6;
    os_sleep_until(app->time);
    mem_clear(&app->tmp);
}
