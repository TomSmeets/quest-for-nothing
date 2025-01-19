// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// monster.h - Monster logic and AI
#pragma once
#include "collision.h"
#include "color.h"
#include "engine.h"
#include "entity.h"
#include "gfx.h"
#include "image.h"
#include "monster_sprite.h"
#include "player.h"
#include "rand.h"
#include "vec.h"

typedef Entity Monster;

static Entity *monster_new(Memory *mem, Random *rng, v3 pos, Sprite_Properties prop) {
    Entity *mon = mem_struct(mem, Entity);
    mon->mtx = m4_id();
    mon->sprite = monster_sprite_generate(mem, prop, rng);
    mon->image = mon->sprite.image;

    // Monster
    mon->is_monster = true;
    mon->pos = pos;
    mon->pos_old = pos;
    mon->size.x = (f32)mon->image->size.x / 32.0f;
    mon->size.y = (f32)mon->image->size.y / 32.0f;
    mon->health = 1 + mon->size.x * mon->size.y * 4;
    mon->shadow = monster_gen_shadow(mem, mon->image->size.x * .8);
    return mon;
}

// Get collision shape
static Shape monster_shape(Entity *mon) {
    return (Shape){.type = Shape_Cylinder, .cylinder = {mon->pos + (v3){0, mon->size.y * .5, 0}, mon->size * .5}};
}

static void monster_update2(Entity *mon) {
}
