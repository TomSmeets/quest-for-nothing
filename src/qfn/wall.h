// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// wall.h - A Quad in 3d space
#pragma once
#include "qfn/collision.h"
#include "qfn/engine.h"
#include "qfn/image.h"
#include "qfn/mat.h"
#include "qfn/monster_sprite.h"

typedef struct Wall Wall;
struct Wall {
    Wall *next;
    m4 mtx;
    Image *image;
};

static Wall *wall_new(Memory *mem, m4 mtx, Image *image) {
    Wall *wall = mem_struct(mem, Wall);

    m4 mtx2 = m4_id();
    m4_scale_image(&mtx2, image);
    m4_apply(&mtx2, mtx);

    wall->mtx = mtx2;
    wall->image = image_copy(mem, image);
    return wall;
}

static void wall_update(Wall *wall, Engine *eng, Collision_World *world) {
    gfx_quad_3d(eng->gfx, wall->mtx, wall->image);
    collision_add(world, wall->mtx, wall->image, 0, wall);
}

static v3 wall_collide(m4 mtx, f32 r, v3 old, v3 new) {
    v3 scale = {v3_length(mtx.x), v3_length(mtx.y), v3_length(mtx.z)};
    v3 radius = scale / 2;
    mtx.x /= scale.x;
    mtx.y /= scale.y;
    mtx.z /= scale.z;

    m4 inv = m4_invert_tr(mtx);
    v3 local_old = m4_mul_pos(inv, old);
    v3 local_new = m4_mul_pos(inv, new);

    // We are behind
    if (local_old.z < 0) return 0;

    v3 point = {local_old.x, local_old.y, 0};
    if (point.x < -radius.x) point.x = -radius.x;
    if (point.x > +radius.x) point.x = +radius.x;
    if (point.y < -radius.y) point.y = -radius.y;
    if (point.y > +radius.y) point.y = +radius.y;

    v3 dir = v3_normalize(local_old - point);
    f32 d_old = v3_dot(dir, local_old - point) - r;
    f32 d_new = v3_dot(dir, local_new - point) - r;
    if (d_new > 0) return 0;
    return m4_mul_dir(mtx, dir * -d_new);
}
