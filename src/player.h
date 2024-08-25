// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// player.h - The player character
#pragma once

#include "input.h"
#include "mat.h"
#include "math.h"

typedef struct {
    v3 pos;
    v3 old_pos;

    // Player rotation around each axis. [0-1]
    v3 rot;

    v3 vel;

    bool on_ground;
    bool flying;

    // Health, probably more satisfying if it is 'discrete' instead of a float
    u32 health;
} Player;

typedef struct {
    v3 move;
    v3 look;
    bool jump;
    bool fly;
} Player_Input;

static Player_Input player_parse_input(Input *input) {
    Player_Input in = {};
    if (key_down(input, KEY_W)) in.move.z -= 1;
    if (key_down(input, KEY_S)) in.move.z += 1;
    if (key_down(input, KEY_A)) in.move.x -= 1;
    if (key_down(input, KEY_D)) in.move.x += 1;
    if (key_down(input, KEY_1)) in.look.z += 1.0f / 8;
    if (key_down(input, KEY_2)) in.look.z -= 1.0f / 8;
    if (key_down(input, KEY_SPACE)) in.jump = 1;
    if (key_down(input, KEY_SPACE)) in.move.y += 1;
    if (key_down(input, KEY_SHIFT)) in.move.y -= 1;
    if (key_click(input, KEY_F)) in.fly = 1;

    if (input->mouse_is_grabbed) {
        in.look.y -= (f32)input->mouse_rel.x / 1000.0f;
        in.look.x -= (f32)input->mouse_rel.y / 1000.0f;
    }
    return in;
}

static void player_update(Player *pl, f32 dt, Player_Input *in) {
    if (in->fly) pl->flying = !pl->flying;

    // Velocity
    // v3 old_pos = pl->old_pos;
    // pl->old_pos = pl->pos;
    v3 old = pl->old_pos;
    pl->old_pos = pl->pos;
    pl->pos += pl->pos - old;

    if (!pl->flying) pl->pos.y -= 9.81 * dt * dt;

    // Look
    pl->rot.xy += in->look.xy;
    pl->rot.x = f_clamp(pl->rot.x, -0.5, 0.5);
    pl->rot.z += (in->look.z - pl->rot.z) * 10 * dt;
    while (pl->rot.y < -1)
        pl->rot.y += 2;
    while (pl->rot.y > 1)
        pl->rot.y -= 2;

    // Render
    m4 player_mtx = m4_id();
    m4_rot_z(&player_mtx, pl->rot.z * PI); // Roll
    m4_rot_x(&player_mtx, pl->rot.x * PI); // Pitch
    m4_rot_y(&player_mtx, pl->rot.y * PI); // Yaw
    m4_trans(&player_mtx, pl->pos);

    // Move
    m4 player_yaw_mtx = m4_id();
    m4_rot_y(&player_yaw_mtx, pl->rot.y * PI); // Yaw
    v3 move = m4s_mul_dir(&player_yaw_mtx.fwd, in->move);
    move.xz = v2_limit(move.xz, 0, 1);
    move.y = in->move.y * pl->flying;

    pl->pos += move * 1.4 * dt;
    if (in->jump && pl->on_ground) {
        pl->old_pos.y = pl->pos.y;
        pl->pos.y += 5 * dt;
    }

    // Collision
    pl->on_ground = false;
    if (pl->pos.y <= 0) {
        pl->pos.y = 0;
        pl->on_ground = true;
    }

    // Friction
    if (pl->on_ground) {
    }

    // Reduce velocity
    v3 vel = pl->pos - pl->old_pos;
    if (pl->flying) {
        vel = v3_limit(vel, 0.01f*dt, 5.0f / 3.6f * dt);
        vel *= 1.0f - 0.2;
    } else {
        vel.xz = v2_limit(vel.xz, 0.01f*dt, 5.0f / 3.6f * dt);
        vel.xz *= 1.0f - 0.2;
    }
    pl->old_pos = pl->pos - vel;
}
