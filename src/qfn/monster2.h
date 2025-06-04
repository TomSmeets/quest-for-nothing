// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// monster2.h - Monster (but new)
#pragma once
#include "lib/vec.h"
#include "qfn/mat.h"
#include "qfn/image.h"
#include "qfn/engine.h"
#include "qfn/monster_sprite.h"

typedef struct Monster Monster;

struct Monster {
    Monster *next;

    v3 pos;
    v2 size;

    Monster_Sprite sprite;
    Image *gun;
    f32 death;
};

static Monster *monster2_new(Memory *mem, v3 pos) {
    Sprite_Properties prop = sprite_new(G->rand);

    Monster *mon = mem_struct(mem, Monster);
    mon->pos = pos;
    mon->sprite = monster_sprite_generate(mem, prop, G->rand);
    mon->size = (v2) { mon->sprite.image->size.x, mon->sprite.image->size.y } / 32.0f;
    return mon;
}

static void m4_scale_image(m4 *mtx, Image *img) {
    m4_scale(mtx, (v3){img->size.x / 32.0f, img->size.y / 32.0f, 1});
}

static void monster2_update(Monster *mon, Engine *eng, v3 player_pos) {
    f32 scared = 0;
    bool dead = mon->death > 0;

    v3 player_dir = player_pos - mon->pos;
    f32 player_dist = v3_length(player_dir);
    if (player_dist < 2) scared = 1 - player_dist / 2;
    if (player_dist < 1) dead = 1;

    if(dead) scared = 0;
    if(dead) mon->death = f_min(mon->death + eng->dt, 1.0f);
    f32 alive = 1 - mon->death;

    // Look around
    f32 look_chance = 0.20f;
    if(scared) look_chance *= 8;
    if (alive && rand_choice(&eng->rng, eng->dt * look_chance)) {
        monster_sprite_update_eyes(&mon->sprite, &eng->rng);
    }

    m4 mtx_monster = m4_id();
    m4_translate(&mtx_monster, mon->pos);

    m4 mtx_rotated = m4_id();
    if (scared) m4_rotate_z(&mtx_rotated, rand_f32(&eng->rng, -1, 1) * .04 * scared);
    if (dead) m4_rotate_x(&mtx_rotated, -R1 * mon->death);
    m4_apply(&mtx_rotated, mtx_monster);
    m4_translate_y(&mtx_rotated, 2e-3f);


    m4 mtx_sprite = m4_id();
    m4_scale_image(&mtx_sprite, mon->sprite.image);
    m4_translate_y(&mtx_sprite, mon->size.y / 2);
    m4_apply(&mtx_sprite, mtx_rotated);

    m4 mtx_shadow = m4_id();
    m4_scale_image(&mtx_shadow, mon->sprite.shadow);
    if (dead) m4_scale(&mtx_shadow, 1 - mon->death);
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
    if (dead) m4_translate_x(&mtx_gun, -0.4 * mon->death);
    if (dead) m4_rotate_z(&mtx_gun, -0.2 * R1 * mon->death);
    m4_apply(&mtx_gun, mtx_rotated);

    // monster_sprite_update_eyes(&mon->sprite, &eng->rng);
    gfx_debug_mtx(eng->gfx_dbg, mtx_rotated);
    gfx_quad_3d(eng->gfx, mtx_sprite, mon->sprite.image);
    gfx_quad_3d(eng->gfx, mtx_shadow, mon->sprite.shadow);
    gfx_quad_3d(eng->gfx, mtx_gun, mon->gun);
}
