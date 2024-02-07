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
#include "rand.h"

struct Camera {
    v3 pos;
    f32 pitch;
    f32 yaw;

    m4 view_to_world;
    m4 world_to_clip;

    v3 old_pos;
    bool on_ground;
    bool can_jump_again;
};


static f32 time_to_snd(u64 us, f32 hz) {
    u64 one_second = 1000000;
    u64 factor = 10;
    return (f32)((us * (u64) (hz * factor)) % (one_second*factor)) / (one_second*factor);
}

static f32 time_seconds(u64 us) {
    return us / 1e6;
}

static f32 snd_pew(u64 us) {
    f32 lfo_amp  = 0.0001;
    f32 lfo_freq = 2;
    f32 compression = 8*2;

    f32 t = time_seconds(us);
    f32 volume = f_max(1 - t*2, 0);
    f32 o = f_sin(time_to_snd(us + lfo_amp*1e6*f_sin(t*lfo_freq), 80*2)*R4)*volume;
    o = f_clamp(o*(volume*compression+1), -1, 1);
    return o;
}

static void cam_update(Camera *cam, Sdl *win, f32 dt) {
    m4 look = m4_id();
    m4_rot_x(&look, -R1);
    m4_rot_z(&look, cam->yaw);
    m4_trans(&look, cam->pos);

    v3 fwd = m4_mul_dir(&look.fwd, (v3){ 0, 0, -1 });
    v3 rgt = m4_mul_dir(&look.fwd, (v3){ 1, 0,  0 });

    // Movement
    v3 move = 0;
    if (input_is_down(&win->input, KEY_W)) move += fwd;
    if (input_is_down(&win->input, KEY_S)) move -= fwd;
    if (input_is_down(&win->input, KEY_A)) move -= rgt;
    if (input_is_down(&win->input, KEY_D)) move += rgt;

    // Velocity
    v3 vel_inst = cam->pos - cam->old_pos;
    cam->old_pos = cam->pos;
    cam->pos += v3_limit(move, 1)*dt*0.60;
    cam->pos += vel_inst - 0.5*dt*dt*(v3){0,0,9.81};
    
    // Floor collision
    cam->pos.xy -= dt*vel_inst.xy*30;
    if(cam->pos.z < 0) {
        cam->pos.z = 0;
        cam->can_jump_again = 0;
        if (input_is_click(&win->input, KEY_SPACE)) {
            cam->can_jump_again = 1;
            cam->pos.z += dt*2;
        }
    } else {
        if (cam->can_jump_again && input_is_click(&win->input, KEY_SPACE)) {
            cam->can_jump_again = 0;
            cam->pos.z += dt*2;
        }
    }

    // Mouse Look
    if (input_is_click(&win->input, KEY_G))
        sdl_grab_mouse(win, !win->has_mouse_grab);

    cam->yaw   += win->input.mouse_rel.x*0.002;
    cam->pitch += win->input.mouse_rel.y*0.002;

    if(cam->pitch < -R1) cam->pitch = -R1;
    if(cam->pitch >  R1) cam->pitch =  R1;

    // Calculate matricies
    cam->view_to_world = m4_id();
    m4_rot_x(&cam->view_to_world, -R1);
    m4_rot_x(&cam->view_to_world, cam->pitch);
    m4_rot_z(&cam->view_to_world, cam->yaw);
    m4_trans(&cam->view_to_world, cam->pos);
    m4_trans(&cam->view_to_world, (v3){0,0,.5});
   
    cam->world_to_clip = m4_id();
    m4_mul_inv(&cam->world_to_clip, &cam->view_to_world);
    m4_perspective_to_clip(&cam->world_to_clip, 45, win->input.window_size.x / win->input.window_size.y, 0.1, 80);

}

struct App {
    mem tmp;
    mem perm;
    Global global;

    Sdl *window;

    // Frame Limiter
    u64 dt;
    u64 time;
    u64 start_time;

    // Animation
    f32 t;

    gl_t *gl;
    UI *ui;

    image *img;
    Camera cam;

    u64 audio_time;
};

static void qfo_audio_callback(void *user, f32 dt, u32 count, v2 *output) {
    App *app = user;
    u64 dt_us = dt * 1e6;
    for(u32 i = 0; i < count; ++i) {
        output[i].y = output[i].x = snd_pew(app->audio_time);
        app->audio_time += dt_us;
    }
}

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
    win->audio_callback = qfo_audio_callback;
    win->audio_user_data = app;

    gl_clear(app->gl, win->input.window_size);

    // Handle quit
    if (win->input.quit || input_is_down(&win->input, KEY_Q)) {
        sdl_quit(win);
        os_exit(0);
    }

    // freecam movement
    cam_update(&app->cam, win, dt);

    // Shooting
    if (input_is_click(&win->input, KEY_MOUSE_LEFT)) {
        os_print("FIRE!\n");
        app->audio_time = 0;
    }

    {
        // Draw something 3d
        Gfx *gfx = gfx_begin(tmp);
        gfx->depth = 1;
        gfx->world_to_clip = app->cam.world_to_clip;

        rand_t rng = {};
        for(u32 i = 0; i < 64*8*4; ++i) {

            f32 angle = rand_f32(&rng)*2*PI;
            f32 theta = f_acos(2*rand_f32(&rng) - 1);

            v4 color = { 1, 1, 1, 1 };
            color = color*.2 + 0.8*(v4){ rand_f32(&rng), rand_f32(&rng), rand_f32(&rng), 1 };
            f32 r = 0.05;

            gfx->mtx = m4_id();
            m4_rot_x(&gfx->mtx, -R1);
            m4_trans(&gfx->mtx, (v3){0, 40, 0});
            m4_rot_x(&gfx->mtx, theta+R1);
            m4_rot_z(&gfx->mtx, angle);
            //m4_rot_z(&gfx->mtx, 0.5*i);
            gfx_color(gfx, color);
            gfx_circle(gfx, (v2){0, 0}, r);

        }


        gfx->mtx = m4_id();
        gfx_color(gfx, (v4){0.1, 0.2, 0.1, 1});
        gfx_circle(gfx, (v2){0, 0}, 10);

        gfx_color(gfx, (v4){0.2, 0.2, 0.1, 1});
        gfx_circle(gfx, (v2){0, 0}, 20);

        gfx->mtx = m4_id();
        m4_scale(&gfx->mtx, (v3){1,1,1}*.2);
        m4_rot_x(&gfx->mtx, -R1); // make text upright
        m4_trans(&gfx->mtx, (v3){0,0,1}); // move into position
        gfx_color(gfx, (v4){1, .5, 0, 1});
        gfx_text(gfx, 0, 1, 1, "The Quick Brown fox jumps\nover the lazy dog");

        gfx->mtx = m4_id();
        m4_scale(&gfx->mtx, (v3){1,1,1}*.2);
        m4_rot_x(&gfx->mtx, -R1); // make text upright
        m4_trans(&gfx->mtx, (v3){0,2,1}); // move into position
        gfx_color(gfx, (v4){0, .5, 0, 1});
        gfx_text(gfx, 0, 1, 1, "The Quick Brown fox jumps\nover the lazy dog");

        gl_draw(app->gl, gfx);
    }

    if(win->has_mouse_grab) {
        // Draw a mouse cursor
        Gfx *gfx = gfx_begin(tmp);
        gfx_color(gfx, RED);
        gfx_circle(gfx, win->input.window_size/2, 4);
        m4_screen_to_clip(&gfx->world_to_clip, win->input.window_size);
        gl_draw(app->gl, gfx);
    } else {
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
