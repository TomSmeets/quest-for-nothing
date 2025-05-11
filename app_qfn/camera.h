// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// camera.h - Camera
#pragma once
#include "engine.h"
#include "input.h"
#include "mat.h"
#include "player.h"

typedef struct {
    v3 pos;
    v3 rot;
    Entity *target;

    f32 screen_shake;
    f32 bob_amount;
    f32 bob_phase;

    // Final Camera Matrix
    m4 mtx;
} Camera;

// Apply screenshake
static void camera_shake(Camera *cam, f32 amount) {
    if (cam->screen_shake < amount) {
        cam->screen_shake = amount;
    }
}

// Set camera target.
// If entity is null, use freecam.
static void camera_follow(Camera *cam, Entity *target) {
    cam->target = target;
}

// Set camera head bob amount
static void camera_bob(Camera *cam, f32 amount) {
    cam->bob_amount = amount;
}

// Apply input
static void camera_input(Camera *cam, Player_Input *input, f32 dt) {
    if (cam->target) {
    } else {
        // Rotate
        cam->rot.x = f_clamp(cam->rot.x + input->look.x, -0.5 * PI, 0.5 * PI);
        cam->rot.y = f_wrap(cam->rot.y + input->look.y, -PI, PI);

        // Apply movement input
        m4 yaw_mtx = m4_id();
        m4_rotate_y(&yaw_mtx, cam->rot.y); // Yaw
        v3 move = m4_mul_dir(yaw_mtx, input->move);
        move = v3_limit(move, 0, 1);
        move.y += input->jump;
        cam->pos += move * 1.4 * dt;
    }
}

static void camera_update(Camera *cam, f32 dt) {
    // Camera Follows player
    if (cam->target) {
        cam->rot = cam->target->rot;
        cam->pos = cam->target->pos;
        cam->pos.y += .5;
    }

    m4 mtx = m4_id();
    if (cam->screen_shake > 0) {
        f32 shake = cam->screen_shake * cam->screen_shake * cam->screen_shake;
        m4_rotate_x(&mtx, f_sin2pi(shake * 3 * 10) * shake * 0.5);
        m4_rotate_z(&mtx, f_sin2pi(shake * 5 * 10) * shake * 0.5);
        m4_rotate_y(&mtx, f_sin2pi(shake * 7 * 10) * shake * 0.5);
        cam->screen_shake -= dt;
    }

    if (cam->target && cam->bob_amount > 0) {
        m4_translate_y(&mtx, f_sin2pi(cam->bob_phase) * cam->bob_amount * 0.01);
        cam->bob_phase += dt * cam->bob_amount * 1.5;
        cam->bob_phase -= (i32)cam->bob_phase;
    }

    m4_rotate(&mtx, cam->rot);
    m4_translate(&mtx, cam->pos);
    cam->mtx = mtx;
}
