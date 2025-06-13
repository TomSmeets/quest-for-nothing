// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// monster2.h - Monster (but new)
#pragma once
#include "lib/vec.h"
#include "qfn/audio.h"
#include "qfn/engine.h"
#include "qfn/image.h"
#include "qfn/mat.h"
#include "qfn/monster_sprite.h"
#include "qfn/sparse_set.h"

typedef struct Monster Monster;

// Idle -> Move -> Idle
// Idle -> Attack -> Shoot -> Flee -> Idle
//
typedef enum {
    // Standing still
    Monster_State_Idle,

    // Moving around
    Monster_State_Move,

    // Attacking player
    Monster_State_Attack,

    // Shooting player
    Monster_State_Shoot,

    // Fleeing
    Monster_State_Flee,

    // Dead
    Monster_State_Dead,
} Monster_State;

struct Monster {
    Monster *next;

    v3 pos;
    v2 size;
    f32 health;

    Monster_State state;
    f32 death_animation;
    f32 wiggle_amount;
    f32 wiggle_phase;
    f32 shoot_timeout;

    // Direction when moving
    v3 move_direction;

    // Visuals
    Monster_Sprite sprite;
    Image *gun;
};

static Monster *monster2_new(Memory *mem, v3 pos, Sprite_Properties prop) {
    Monster *mon = mem_struct(mem, Monster);
    mon->pos = pos;
    mon->sprite = monster_sprite_generate(mem, prop, G->rand);
    mon->size = (v2){mon->sprite.image->size.x, mon->sprite.image->size.y} / 32.0f;
    mon->health = 1 + mon->size.x * mon->size.y * 16;
    return mon;
}

static void monster2_update(Monster *mon, Engine *eng, Audio *audio, Sparse_Set *sparse, v3 player_pos) {
    f32 dt = eng->dt;
    Rand *rng = &eng->rng;

    v3 player_diff = player_pos - mon->pos;
    f32 player_dist = v3_length(player_diff);
    v3 player_dir = player_dist > 0 ? player_diff / player_dist : 0;

    // State switching
    // Idle -> Move
    if (mon->state == Monster_State_Idle && rand_choice(rng, 0.4 * dt)) {
        mon->state = Monster_State_Move;
        f32 angle = rand_f32(rng, -1, 1);
        mon->move_direction = (v3){f_cos2pi(angle), 0, f_sin2pi(angle)};
    }

    // Move -> Idle
    else if (mon->state == Monster_State_Move && rand_choice(rng, 0.4 * dt)) {
        mon->state = Monster_State_Idle;
    }

    // Idle -> Attack
    else if (mon->state == Monster_State_Idle && rand_choice(rng, 0.4 * dt)) {
        mon->state = Monster_State_Attack;
    }

    // Attack -> Shoot
    else if (mon->state == Monster_State_Attack && rand_choice(rng, f_remap(player_dist, 0.5, 2, 1, dt * 0.5))) {
        mon->state = Monster_State_Shoot;
    }

    // Shoot -> Flee
    else if (mon->state == Monster_State_Shoot && rand_choice(rng, 0.5 * dt)) {
        mon->state = Monster_State_Flee;
    }

    // Flee -> Idle
    else if (mon->state == Monster_State_Flee && rand_choice(rng, 0.5 * dt)) {
        mon->state = Monster_State_Idle;
    }

    f32 scared_amount = f_remap(player_dist, 0, 2, 1, 0);
    f32 look_chance = f_remap(scared_amount, 0, 1, 0.20, 2.00);
    v3 vel = 0;

    // Idle
    if (mon->state == Monster_State_Idle) {
    }

    // Move
    else if (mon->state == Monster_State_Move) {
        vel = mon->move_direction * 0.4;
    }

    // Attack
    else if (mon->state == Monster_State_Attack) {
        vel = player_dir * 0.8;
        vel.y = 0;
    }

    // Shoot
    else if (mon->state == Monster_State_Shoot) {
        if (mon->shoot_timeout == 0) {
            mon->shoot_timeout = 1.0f;
            // audio->play_shoot = 1;
        }
    }

    // Flee
    else if (mon->state == Monster_State_Flee) {
        vel = -player_dir;
        vel.y = 0;
    }

    // Dead
    else if (mon->state == Monster_State_Dead) {
        look_chance = 0.0f;
        mon->death_animation = f_min(mon->death_animation + dt, 1.0f);
    }

    mon->shoot_timeout -= dt * 2;
    if (mon->shoot_timeout < 0) mon->shoot_timeout = 0;

    mon->wiggle_amount += (v3_length(vel) * .5 - mon->wiggle_amount) * dt * 2;
    mon->wiggle_phase = f_fract(mon->wiggle_phase + dt * mon->wiggle_amount * 5);

    f32 dead_amount = mon->death_animation;
    f32 wiggle_amount = mon->wiggle_amount;
    f32 wiggle_phase = mon->wiggle_phase;

    // Look around
    if (look_chance && rand_choice(rng, dt * look_chance)) {
        monster_sprite_update_eyes(&mon->sprite, rng);
    }

    // Movement
    mon->pos += vel * dt;

    // Collision
    f32 r = mon->size.x / 2;
    f32 h = mon->size.y;
    Box box = {mon->pos - (v3){r, 0, r}, mon->pos + (v3){r, h, r}};
    gfx_debug_box(eng->gfx_dbg, box, 0);

    // Graphics
    m4 mtx_monster = m4_id();
    m4_translate(&mtx_monster, mon->pos);

    m4 mtx_rotated = m4_id();
    if (scared_amount) m4_translate_x(&mtx_rotated, rand_f32(rng, -1, 1) * .01 * scared_amount);
    if (wiggle_amount) m4_rotate_z(&mtx_rotated, f_sin(wiggle_phase * R4) * R1 * wiggle_amount * 0.2);
    if (dead_amount) m4_rotate_x(&mtx_rotated, -R1 * dead_amount);
    m4_rotate_y(&mtx_rotated, R1 - f_atan2(player_dir.z, player_dir.x));
    m4_apply(&mtx_rotated, mtx_monster);
    m4_translate_y(&mtx_rotated, 2e-3f);

    m4 mtx_sprite = m4_id();
    m4_scale_image(&mtx_sprite, mon->sprite.image);
    m4_translate_y(&mtx_sprite, mon->size.y / 2);
    m4_apply(&mtx_sprite, mtx_rotated);

    m4 mtx_shadow = m4_id();
    m4_scale_image(&mtx_shadow, mon->sprite.shadow);
    if (dead_amount) m4_scale(&mtx_shadow, 1 - dead_amount);
    m4_rotate_x(&mtx_shadow, -R1);
    m4_translate_y(&mtx_shadow, 1e-3f);
    m4_apply(&mtx_shadow, mtx_monster);

    m4 mtx_gun = m4_id();
    f32 gun_y = mon->size.y - mon->sprite.hand[0].y / 32.0f;
    f32 gun_x = -mon->sprite.hand[0].x / 32.0f * 0.5f * 0.9f;
    m4_scale_image(&mtx_gun, mon->gun);
    m4_translate_x(&mtx_gun, -.1);
    m4_rotate_z(&mtx_gun, f_remap(mon->shoot_timeout, 0, 1, 0, -0.2 * R1));
    m4_rotate_y(&mtx_gun, R1 * 0.80f * (1 - dead_amount));
    m4_translate_y(&mtx_gun, gun_y);
    m4_translate_x(&mtx_gun, gun_x);
    if (dead_amount) m4_translate_x(&mtx_gun, -0.4 * dead_amount);
    if (dead_amount) m4_rotate_z(&mtx_gun, -0.2 * R1 * dead_amount);
    m4_apply(&mtx_gun, mtx_rotated);

    // monster_sprite_update_eyes(&mon->sprite, rng);
    // gfx_debug_mtx(eng->gfx_dbg, mtx_rotated);
    gfx_quad_3d(eng->gfx, mtx_sprite, mon->sprite.image);
    gfx_quad_3d(eng->gfx, mtx_shadow, mon->sprite.shadow);
    gfx_quad_3d(eng->gfx, mtx_gun, mon->gun);
}
