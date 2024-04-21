// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// quest_for_nothing.c - Main entry point for "Quest For Nothing"
#include "tlib/inc.h"
#include "tlib/color.h"
#include "tlib/gl.h"
#include "tlib/input.h"
#include "tlib/math.h"
#include "tlib/mem.h"
#include "tlib/sdl.h"
#include "tlib/vec.h"
#include "tlib/mat.h"
#include "tlib/sound.h"
#include "tlib/gfx.h"
#include "tlib/ui.h"
#include "tlib/parse_qoi.h"
#include "tlib/rand.h"
#include "tlib/os_generic.h"
#include "app/quest_for_nothing/monster.h"
#include "app/quest_for_nothing/data.h"

static Level *level_new() {
    mem mem = {};
    Level *lvl = mem_struct(&mem, Level);
    lvl->mem = mem;
    return lvl;
}

static void level_destroy(Level *lvl) {
    mem mem = lvl->mem;
    mem_clear(&mem);
}

static Monster *app_spawn_monster(Level *lvl, rand_t *rng) {
    Monster *mon = mem_struct(&lvl->mem, Monster);
    mon->life = 1;
    mon->pos    = rand_v2(rng)*20;
    mon->radius = .5;
    mon->height = 1;
    mon->next = lvl->monster_list;
    lvl->monster_list = mon;
    return mon;
}

static void app_step_monster_spawner(App *app) {
    Level *lvl = app->level;
    while(lvl->next_monster_spawn_time < lvl->time) {
        lvl->next_monster_spawn_time += 1;
        app_spawn_monster(app->level, &app->rng);
    }
}

static u32 app_step_monsters(App *app, Gfx *gfx) {
    u32 dead_count = 0;
    Level *lvl = app->level;
    for(Monster *mon = lvl->monster_list; mon; mon = mon->next) {
        mon_update(app, mon, gfx);
        if(mon->life <= 0) dead_count++;
    }
    return dead_count;
}

static void player_update(App *app, Player *player, Sdl *win) {
    m4 look = m4_id();
    m4_rot_x(&look, -R1);
    m4_rot_z(&look, player->yaw);
    m4_trans(&look, player->pos);

    v3 fwd = m4_mul_dir(&look.fwd, (v3){ 0, 0, -1 });
    v3 rgt = m4_mul_dir(&look.fwd, (v3){ 1, 0,  0 });

    // Movement
    v3 move = 0;
    if (input_is_down(&win->input, KEY_W)) move += fwd;
    if (input_is_down(&win->input, KEY_S)) move -= fwd;
    if (input_is_down(&win->input, KEY_A)) move -= rgt;
    if (input_is_down(&win->input, KEY_D)) move += rgt;

    f32 speed = 1;
    if (input_is_down(&win->input, KEY_SHIFT)) speed = 2;

    // Velocity
    v3 vel_inst = player->pos - player->old_pos;
    player->old_pos = player->pos;
    player->pos += v3_limit(move, 1)*app->dt*0.60*speed;
    player->pos += vel_inst - 0.5*app->dt*app->dt*(v3){0,0,9.81};
    
    // Floor collision
    player->pos.xy -= app->dt*vel_inst.xy*30;
    if(player->pos.z < 0) {
        player->pos.z = 0;
        player->can_jump_again = 0;
        if (input_is_click(&win->input, KEY_SPACE)) {
            player->can_jump_again = 1;
            player->pos.z += app->dt*2;
            snd_play_jump(&app->sound, 0);
        }
    } else {
        if (player->can_jump_again && input_is_click(&win->input, KEY_SPACE)) {
            player->can_jump_again = 0;
            // Reset z velocity
            player->old_pos.z = player->pos.z;
            player->pos.z += app->dt*2;
            snd_play_jump(&app->sound, 1);
        }
    }

    // Mouse Look
    if (input_is_click(&win->input, KEY_G))
        sdl_grab_mouse(win, !win->has_mouse_grab);

    player->yaw   += win->input.mouse_rel.x*0.002;
    player->pitch += win->input.mouse_rel.y*0.002;

    if(player->pitch < -R1) player->pitch = -R1;
    if(player->pitch >  R1) player->pitch =  R1;

    // Calculate matricies
    player->view_to_world = m4_id();
    m4_rot_x(&player->view_to_world, -R1);
    m4_rot_x(&player->view_to_world, player->pitch);
    m4_rot_z(&player->view_to_world, player->yaw);
    m4_trans(&player->view_to_world, player->pos);
    m4_trans(&player->view_to_world, (v3){0,0,.5});
   
    player->world_to_clip = m4_id();
    m4_mul_inv(&player->world_to_clip, &player->view_to_world);
    m4_perspective_to_clip(&player->world_to_clip, 45, win->input.window_size.x / win->input.window_size.y, 0.1, 80);
}

// You can choose how to run this app
// - dynamically: use ./hot main.so
// - directly:    use ./main
void *main_init(int argc, char **argv) {
    u32 fps = 200;

    mem m = {};
    App *app = mem_struct(&m, App);
    global_set(&app->global);
    app->perm = m;
    app->window = sdl_new(&m, "Hello World");
    app->start_time = os_time();
    app->gl   = gl_init(&m, app->window->gl);
    app->dt_us   = 1000 * 1000 / fps;
    app->time_us = app->start_time;
    app->dt = (f32) 1.0f / fps;
    app->ui   = mem_struct(&m, UI);
    app->img = parse_qoi(&m, os_read_file(&m, "res/space_alien.qoi"));

    Level *lvl = level_new();
    app->level = lvl;

    return app;
}


void main_update(void *handle) {
    App *app = handle;
    mem *tmp = &app->tmp;

    Sdl *win = app->window;

    // Little bit ugly
    global_set(&app->global);

    // Create a window
    sdl_begin(win);
    sdl_audio(win, snd_system_callback, &app->sound);
    gl_clear(app->gl, win->input.window_size);

    // Handle quit
    if (win->input.quit || input_is_down(&win->input, KEY_Q)) {
        sdl_quit(win);
        os_exit(0);
    }

    // freecam movement
    player_update(app, &app->player, win);

    ui_begin(app->ui, &win->input, tmp);
    // Shooting
    if (input_is_click(&win->input, KEY_MOUSE_LEFT)) {
        os_print("FIRE!\n");
        snd_play_pew(&app->sound, rand_f32(&app->rng));

        f32 min_dist = 1000;
        Monster *min_mon = 0;
        for(Monster *mon = app->level->monster_list; mon; mon = mon->next) {
            guard(mon->life > 0);
            v3 d = m4_mul_pos(&app->player.view_to_world.inv, mon->pos);
            f32 dist = -d.z;
            if(d.x > -.5 && d.x < .5 && d.y <= 1 && d.y > 0 && dist >= 0 && dist < min_dist) {
                min_mon = mon;
                min_dist = dist;
            }
        }
        if(min_mon) {
            min_mon->life = 0;
            snd_play_squish(&app->sound);
        }
    }

    {
        // Draw something 3d
        Gfx *gfx = gfx_begin(tmp);
        gfx->depth = 1;
        gfx->world_to_clip = app->player.world_to_clip;

        rand_t rng = {};
        for(u32 i = 0; i < 64*8*4; ++i) {

            f32 angle = rand_f32(&rng)*2*PI;
            f32 theta = f_acos(2*rand_f32(&rng) - 1);

            v4 color = { 1, 1, 1, 1 };
            // color = color*.2 + 0.8*(v4){ rand_f32(&rng), rand_f32(&rng), rand_f32(&rng), 1 };
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
        gfx_color(gfx, (v4){0.01, 0.02, 0.01, 1});
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

        // Image *i = img_new_uninit(tmp, 8, 8);
        // img_fill_pattern(i);

        u32 dead_count = app_step_monsters(app, gfx);
        app_step_monster_spawner(app);
        gl_draw(app->gl, gfx);

        {
            Gfx *gfx = gfx_begin(tmp);
            gfx_color(gfx, WHITE);
            gfx_text(gfx, (v2) {20, 20}, 40, 40, fmt(tmp, "0123456789: %2d%s", dead_count, "%"));
            m4_screen_to_clip(&gfx->world_to_clip, win->input.window_size);
            gl_draw(app->gl, gfx);
        }
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
    app->time_us += app->dt_us;
    app->time    += app->dt;
    app->level->time += app->dt;
    os_sleep_until(app->time);
    mem_clear(&app->tmp);
}
