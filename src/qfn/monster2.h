// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// monster2.h - Monster (but new)
#pragma once
#include "lib/vec.h"
#include "qfn/engine.h"
#include "qfn/image.h"
#include "qfn/mat.h"
#include "qfn/monster_sprite.h"

typedef struct Monster Monster;

typedef enum {
    Monster_State_Idle,
    Monster_State_Attack,
    Monster_State_Scared,
    Monster_State_Dead,
} Monster_State;

struct Monster {
    Monster *next;

    f32 state_time;
    Monster_State state;

    v3 pos;
    v2 size;
    f32 health;

    // Visuals
    Monster_Sprite sprite;
    Image *gun;
};

static Monster *monster2_new(Memory *mem, v3 pos) {
    Sprite_Properties prop = sprite_new(G->rand);

    Monster *mon = mem_struct(mem, Monster);
    mon->pos = pos;
    mon->sprite = monster_sprite_generate(mem, prop, G->rand);
    mon->size = (v2){mon->sprite.image->size.x, mon->sprite.image->size.y} / 32.0f;
    mon->health = 1 + mon->size.x * mon->size.y * 16;
    return mon;
}

static void m4_scale_image(m4 *mtx, Image *img) {
    m4_scale(mtx, (v3){img->size.x / 32.0f, img->size.y / 32.0f, 1});
}

static void monster2_set_state(Monster *mon, Monster_State state) {
    mon->state = state;
    mon->state_time = 0;
}

static void monster2_update(Monster *mon, Engine *eng, v3 player_pos) {
    f32 dt = eng->dt;
    Rand *rng = &eng->rng;

    v3 player_dir = player_pos - mon->pos;
    f32 player_dist = v3_length(player_dir);
    if (player_dist < 2) monster2_set_state(mon, Monster_State_Scared);
    if (player_dist < 1) monster2_set_state(mon, Monster_State_Dead);

    bool idle   = mon->state == Monster_State_Idle;
    bool attack = mon->state == Monster_State_Attack;
    f32 dead = mon->state == Monster_State_Dead ? mon->state_time : 0;
    f32 scared = mon->state == Monster_State_Scared ? (1 - player_dist / 2) : 0;
    f32 alive = 1 - dead;

    mon->state_time = f_min(mon->state_time + dt, 1.0f);

    // Look around
    f32 look_chance = 0.20f;
    if (scared) look_chance = 2.0f;
    if (dead)   look_chance = 0;
    if (look_chance && rand_choice(rng, dt * look_chance)) {
        monster_sprite_update_eyes(&mon->sprite, rng);
    }

    if (scared && rand_choice(rng, dt * 0.5)) monster2_set_state(mon, Monster_State_Idle);
    if (attack && rand_choice(rng, dt * 0.1)) monster2_set_state(mon, Monster_State_Idle);
    if (idle && rand_choice(rng, dt * 0.1)) monster2_set_state(mon, Monster_State_Attack);

    v3 vel = 0;
    if (attack && player_dist > 0) vel += player_dir / player_dist * 0.20f;
    if (scared && player_dist > 0) vel -= player_dir / player_dist * 1.00f;
    mon->pos += vel * dt;

    m4 mtx_monster = m4_id();
    m4_translate(&mtx_monster, mon->pos);

    m4 mtx_rotated = m4_id();
    // if (!dead) m4_rotate_z(&mtx_rotated, f_sin(mon->state_time*R4) * R1 * 0.3);
    if (scared) m4_rotate_z(&mtx_rotated, rand_f32(rng, -1, 1) * .08 * scared);
    if (dead) m4_rotate_x(&mtx_rotated, -R1 * dead);
    m4_rotate_y(&mtx_rotated, R1-f_atan2(player_dir.z, player_dir.x));
    m4_apply(&mtx_rotated, mtx_monster);
    m4_translate_y(&mtx_rotated, 2e-3f);

    m4 mtx_sprite = m4_id();
    m4_scale_image(&mtx_sprite, mon->sprite.image);
    m4_translate_y(&mtx_sprite, mon->size.y / 2);
    m4_apply(&mtx_sprite, mtx_rotated);

    m4 mtx_shadow = m4_id();
    m4_scale_image(&mtx_shadow, mon->sprite.shadow);
    if (dead) m4_scale(&mtx_shadow, alive);
    m4_rotate_x(&mtx_shadow, -R1);
    m4_translate_y(&mtx_shadow, 1e-3f);
    m4_apply(&mtx_shadow, mtx_monster);

    m4 mtx_gun = m4_id();
    f32 gun_y = mon->size.y - mon->sprite.hand[0].y / 32.0f;
    f32 gun_x = -mon->sprite.hand[0].x / 32.0f * 0.5f * 0.9f;
    m4_scale_image(&mtx_gun, mon->gun);
    m4_translate_x(&mtx_gun, -.1);
    m4_rotate_y(&mtx_gun, R1 * 0.80f * alive);
    m4_translate_y(&mtx_gun, gun_y);
    m4_translate_x(&mtx_gun, gun_x);
    if (dead) m4_translate_x(&mtx_gun, -0.4 * dead);
    if (dead) m4_rotate_z(&mtx_gun, -0.2 * R1 * dead);
    m4_apply(&mtx_gun, mtx_rotated);

    // monster_sprite_update_eyes(&mon->sprite, rng);
    // gfx_debug_mtx(eng->gfx_dbg, mtx_rotated);
    gfx_quad_3d(eng->gfx, mtx_sprite, mon->sprite.image);
    gfx_quad_3d(eng->gfx, mtx_shadow, mon->sprite.shadow);
    gfx_quad_3d(eng->gfx, mtx_gun, mon->gun);
}
