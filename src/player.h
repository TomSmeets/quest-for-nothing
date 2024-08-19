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

    // Health, probably more satisfying if it is 'discrete' instead of a float
    u32 health;
} Player;

static void player_look(Player *pl, f32 yaw, f32 pitch) {
    pl->rot.x += pitch;
    pl->rot.y += yaw;

    pl->rot.x = f_clamp(pl->rot.x, -0.5, 0.5);
    while (pl->rot.y < -1)
        pl->rot.y += 2;
    while (pl->rot.y > 1)
        pl->rot.y -= 2;
}

typedef struct {
    v3 move;
    v3 look;
    bool jump;
} Player_Input;

static void player_update(Player *pl, f32 dt, Player_Input *in) {
    // Velocity
    // v3 old_pos = pl->old_pos;
    // pl->old_pos = pl->pos;
    v3 old = pl->old_pos;
    pl->old_pos = pl->pos;
    pl->pos += (pl->pos - old) - (v3){0, 9.81 * dt * dt, 0};

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
    v3 move = m4s_mul_dir(&player_mtx.fwd, in->move);
    move.xy = v2_limit(move.xy, 1);
    move.y = 0;
    pl->pos += move * 5 / 3.6 * dt;
    if (in->jump && pl->on_ground) {
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
    pl->old_pos.xz -= (pl->old_pos.xz - pl->pos.xz) * 0.5; //*8*dt;
    // pl->pos -= (pl->pos - pl->old_pos)*0.5;
}
