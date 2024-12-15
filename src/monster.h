// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// monster.h - Monster logic and AI
#pragma once
#include "color.h"
#include "engine.h"
#include "gfx.h"
#include "image.h"
#include "monster_sprite.h"
#include "player.h"
#include "rand.h"
#include "vec.h"
#include "entity.h"

typedef Entity Monster;

static Entity *monster_new(Memory *mem, Random *rng, v3 pos) {
    Monster_Sprite sprite = monster_sprite_generate(mem, rng);

    Entity *mon = mem_struct(mem, Entity);
    mon->mtx = m4_id();
    mon->img = sprite.image;
    
    // Monster
    mon->is_monster = true;
    mon->pos = pos;
    mon->pos_old = pos;
    mon->health = 10;
    mon->eye_count = sprite.eye_count;
    for (u32 i = 0; i < sprite.eye_count; ++i) mon->eye[i] = sprite.eye[i];
    mon->shadow = monster_gen_shadow(mem, sprite.image->size.x * .5);

    // AI
    mon->is_ai = true;
    return mon;
}
