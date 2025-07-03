// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// player.h - The player character
#pragma once
#include "lib/vec.h"
#include "qfn/audio.h"
#include "qfn/collision.h"
#include "qfn/engine.h"
#include "qfn/input.h"
#include "qfn/monster.h"
#include "qfn/wall.h"

typedef struct {
    v3 move;
    v3 look;

    bool jump;
    bool fly;
    bool shoot;
    bool shoot_single;
} Player_Input;

// Map Keys and mouse movement to player input
static Player_Input player_parse_input(Input *input) {
    Player_Input in = {};
    if (input_down(input, KEY_W)) in.move.z += 1;
    if (input_down(input, KEY_S)) in.move.z -= 1;
    if (input_down(input, KEY_A)) in.move.x += 1;
    if (input_down(input, KEY_D)) in.move.x -= 1;

    if (input_down(input, KEY_I)) in.look.x -= 0.015;
    if (input_down(input, KEY_K)) in.look.x += 0.015;
    if (input_down(input, KEY_J)) in.look.y += 0.015;
    if (input_down(input, KEY_L)) in.look.y -= 0.015;

    if (input_down(input, KEY_1)) in.look.z += 1.0f / 8;
    if (input_down(input, KEY_2)) in.look.z -= 1.0f / 8;
    if (input_down(input, KEY_SPACE)) in.jump = 1;
    if (input_down(input, KEY_MOUSE_LEFT)) in.shoot = 1;
    if (input_click(input, KEY_MOUSE_LEFT)) in.shoot_single = 1;
    if (input_down(input, KEY_SPACE)) in.move.y += 1;
    if (input_down(input, KEY_SHIFT)) in.move.y -= 1;

    if (input->mouse_is_grabbed) {
        in.look.y -= (f32)input->mouse_rel.x / 1000.0f;
        in.look.x -= (f32)input->mouse_rel.y / 1000.0f;
    }
    return in;
}

typedef struct {
    v3 pos;
    v3 old;
    v3 look;
    Image *gun;
    f32 shoot_timeout;

    f32 screen_shake;
    f32 bob_amount;
    f32 bob_phase;

    m4 camera;
    bool fly;
} Player;

static Player *player_new(Memory *mem, v3 pos) {
    Player *player = mem_struct(mem, Player);
    player->pos = pos;
    player->gun = gun_new(mem, G->rand);
    return player;
}

static void player_update(Player *player, Collision_World *world, Engine *eng, Audio *audio) {
    Player_Input input = player_parse_input(eng->input);
    if (input_click(eng->input, KEY_3)) player->fly = !player->fly;

    player->look.xy += input.look.xy;
    player->look.x = f_clamp(player->look.x, -PI / 2, PI / 2);
    player->look.y = f_wrap(player->look.y, -PI, PI);
    player->look.z += (input.look.z - player->look.z) * 10 * eng->dt * PI;

    m4 mtx_yaw = m4_id();
    m4_rotate_y(&mtx_yaw, player->look.y);

    v3 move = m4_mul_dir(mtx_yaw, input.move);
    move.xz = v2_limit(move.xz, 0, 1);

    v3 old = player->pos;
    if (player->fly) {
        player->pos += move * 2 * eng->dt;
        player->old = player->pos;
        player->bob_amount = 0;
    } else {
        move.y = 0;
        player->bob_amount += (v3_length(move) * 2 - player->bob_amount) * eng->dt * 8;

        v3 vel = player->pos - player->old;
        player->old = player->pos;
        player->pos += move * 2.0 * eng->dt;
        player->pos.y += vel.y;
        player->pos.y -= 10 * eng->dt * eng->dt;

        // Gravity
        bool on_ground = false;
        if (player->pos.y <= 0) {
            on_ground = true;
            player->pos.y = 0;
        }

        // Jumping
        if (input.jump && on_ground) {
            player->pos.y += eng->dt * 4;
            audio->play_jump = 1;
        }

        // Collision
        for (Collision_Object *obj = world->objects; obj; obj = obj->next) {
            if (obj->type != 0) continue;
            f32 r = 0.25;
            v3 offset = {0, r, 0};
            player->pos += wall_collide(obj->mtx, r, old + offset, player->pos + offset);
        }
    }

    bool did_shoot = 0;
    player->shoot_timeout = f_max(player->shoot_timeout - eng->dt * 2, 0);
    if (input.shoot && player->shoot_timeout == 0) {
        player->shoot_timeout = 1;
        audio->play_shoot = 1;
        player->screen_shake += .5;
        did_shoot = 1;
    }

    m4 mtx_body = m4_id();
    m4_apply(&mtx_body, mtx_yaw);
    m4_translate(&mtx_body, player->pos);

    m4 mtx_head = m4_id();
    m4_rotate_z(&mtx_head, player->look.z);
    m4_rotate_x(&mtx_head, player->look.x);
    m4_translate_y(&mtx_head, 0.5f);
    m4_apply(&mtx_head, mtx_body);

    m4 mtx_gun = m4_id();
    m4_scale_image(&mtx_gun, player->gun);
    m4_translate_x(&mtx_gun, -0.2);
    m4_rotate_z(&mtx_gun, f_remap(player->shoot_timeout, 0, 1, 0, -0.2 * R1));
    m4_rotate_y(&mtx_gun, R1);
    m4_translate_x(&mtx_gun, -0.20);
    m4_translate_z(&mtx_gun, 0.15);
    m4_translate_y(&mtx_gun, -0.12);
    m4_apply(&mtx_gun, mtx_head);
    gfx_quad_3d(eng->gfx, mtx_gun, player->gun);

    m4 mtx_camera = m4_id();
    if (player->screen_shake > 0) {
        f32 shake = player->screen_shake * player->screen_shake * player->screen_shake;
        m4_rotate_x(&mtx_camera, f_sin2pi(shake * 3 * 10) * shake * 0.5);
        m4_rotate_z(&mtx_camera, f_sin2pi(shake * 5 * 10) * shake * 0.5);
        m4_rotate_y(&mtx_camera, f_sin2pi(shake * 7 * 10) * shake * 0.5);
        player->screen_shake -= eng->dt;
    }

    if (player->bob_amount > 0) {
        m4_translate_y(&mtx_camera, f_sin2pi(player->bob_phase) * player->bob_amount * 0.01);
        player->bob_phase += eng->dt * player->bob_amount * 1.5;
        player->bob_phase -= (i32)player->bob_phase;
    }

    m4_apply(&mtx_camera, mtx_head);
    player->camera = mtx_camera;

    if (did_shoot) {
        u32 n = 32 * 4;
        for (u32 i = 0; i < n; ++i) {
            Collide_Result hit_res = {.distance = 1000.0f};
            Collision_Object *hit_obj = 0;

            f32 shot_ang = rand_f32(&eng->rng, 0, 1);
            f32 shot_dist = rand_f32(&eng->rng, 0, .1f);
            m4 shoot_mtx = mtx_head;
            v3 shoot_pos = shoot_mtx.w;
            v3 shoot_dir = shoot_mtx.z;
            shoot_dir += shoot_mtx.x * f_cos2pi(shot_ang) * shot_dist;
            shoot_dir += shoot_mtx.y * f_sin2pi(shot_ang) * shot_dist;
            for (Collision_Object *obj = world->objects; obj; obj = obj->next) {
                Collide_Result res;
                if (collide_quad_ray(&res, obj->mtx, shoot_pos, shoot_dir)) {
                    Image *img = obj->img;
                    v4 *px = image_get(img, (v2i){(res.uv.x + .5) * img->size.x, (.5 - res.uv.y) * img->size.y});

                    if (!px) continue;
                    if (px->w < .9f) continue;
                    if (res.distance > hit_res.distance) continue;
                    hit_obj = obj;
                    hit_res = res;
                }
            }

            if (hit_obj) {
                Image *img = hit_obj->img;
                v4 *px = image_get(img, (v2i){(hit_res.uv.x + .5) * img->size.x, (.5 - hit_res.uv.y) * img->size.y});
                if (px) {
                    f32 t = (f32)(eng->time.frame_start / 1000 / 1000 % 60) / 60;
                    px->x += ((f_cos2pi(t + 0.0f / 3.0f) + 1) / 2 - px->x) * .9f;
                    px->y += ((f_cos2pi(t + 1.0f / 3.0f) + 1) / 2 - px->y) * .9f;
                    px->z += ((f_cos2pi(t + 2.0f / 3.0f) + 1) / 2 - px->z) * .9f;
                    px->w = 1;
                    img->variation++;
                }
                if (hit_obj->type == 1) {
                    Monster *mon = hit_obj->handle;
                    mon->health -= 10.0f / n;
                }
            }
        }
    }
}
