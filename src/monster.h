// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// monster.h - Monster logic and AI
#pragma once
#include "collision.h"
#include "color.h"
#include "engine.h"
#include "entity.h"
#include "gfx.h"
#include "gfx_debug.h"
#include "image.h"
#include "monster_sprite.h"
#include "player.h"
#include "rand.h"
#include "sparse_set.h"
#include "vec.h"

#define GFX_PIXEL_SCALE_UI 4.0f
#define GFX_PIXEL_SCALE_3D (1.0f / 32.0f)

static void m4_image_at_scale(m4 *mtx, Image *img, f32 scale) {
    v2 size = {img->size.x, img->size.y};
    v2 origin = {img->origin.x, img->origin.y};
    size *= scale;
    origin *= scale;

    // Scale to image size, 1 unit = 1 pixel
    m4_scale(mtx, (v3){size.x, size.y, 1});

    // Center at origin
    m4_translate_x(mtx, 0.5 * size.x - origin.x);
    m4_translate_y(mtx, origin.y - 0.5 * size.y);
}

static void m4_image_3d(m4 *mtx, Image *img) {
    m4_image_at_scale(mtx, img, GFX_PIXEL_SCALE_3D);
}

static void m4_image_ui(m4 *mtx, Image *img) {
    m4_image_at_scale(mtx, img, GFX_PIXEL_SCALE_UI);
}

#define SHADOW_OFFSET 0.01
#define MONSTER_OFFSET 0.02

static Entity *monster_new(Memory *mem, Random *rng, v3 pos, Sprite_Properties prop) {
    Entity *mon = mem_struct(mem, Entity);
    mon->type = Entity_Monster;
    mon->mtx = m4_id();
    mon->sprite = monster_sprite_generate(mem, prop, rng);
    mon->image = mon->sprite.image;

    // Monster
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

static void monster_wiggle(Entity *mon, Engine *eng) {
    // ==== Animation ====
    f32 speed = f_min(v3_length(mon->vel), 0.6);
    animate_exp(&mon->wiggle_amp, speed, eng->dt * 4);
    mon->wiggle_phase = f_fract(mon->wiggle_phase + mon->wiggle_amp * 0.08);
}

static void monster_die(Entity *mon, Engine *eng) {
    mon->wiggle_amp = 0;
    mon->wiggle_phase = 0;
    animate_lin(&mon->death_animation, 1, eng->dt * 4);
}

static void monster_update_eyes(Entity *mon, Engine *eng) {
    if (animate2(&mon->look_around_timer, eng->dt)) {
        mon->look_around_timer = rand_f32_range(&eng->rng, 1, 8);
        monster_sprite_update_eyes(&mon->sprite, &eng->rng);
    }
}

static void monster_update_ai(Entity *mon, Entity *player, Engine *eng) {
    // Apply movement
    mon->pos.xz += mon->move_dir.xy * eng->dt;

    // AI movement
    if (animate2(&mon->move_time, eng->dt)) {
        mon->move_time = rand_f32_range(&eng->rng, 2, 10);

        // AI Movement mode
        u32 mode = rand_u32_range(&eng->rng, 0, 2);

        // Stand still
        if (mode == 0) mon->move_dir = 0;

        // To player
        if (mode == 1) mon->move_dir = v2_normalize(player->pos.xz - mon->pos.xz) * 0.1;

        // Random direction
        if (mode == 2) mon->move_dir = v2_from_rot(rand_f32_signed(&eng->rng) * PI) * 0.25;
    }

    // Look direction
    mon->look_dir = v3_normalize((player->pos - mon->pos) * (v3){1, 0, 1});
}

static void entity_update_movement(Entity *mon, Engine *eng) {
    bool do_gravity = !mon->is_flying;
    bool do_ground_collision = !mon->is_flying;

    // Reset state
    mon->on_ground = false;

    // Physics
    v3 vel_dt = mon->pos - mon->pos_old;
    mon->vel = vel_dt / eng->dt;
    mon->pos_old = mon->pos;

    if (do_gravity) {
        mon->pos.y -= 9.81 * eng->dt * eng->dt;
        mon->pos.y += vel_dt.y;
    }

    if (do_ground_collision && mon->pos.y < 0) {
        mon->pos.y = 0;
        mon->on_ground = true;
    }
}

static void entity_collide(Engine *eng, Sparse_Set *sparse, Entity *mon) {
    // Construct a bounding box around the monster
    Shape shape = monster_shape(mon);
    Box box = box_from_shape(shape);

    // Add entity to the BVH
    sparse_set_add(sparse, box, mon);

    // Check all colliding bounding boxes
    for (Sparse_Collision *col = sparse_set_check(sparse, box); col; col = col->next) {
        Entity *ent = col->node->user;

        // Skip collisions with myself
        if (ent == mon) continue;

        if (ent->type == Entity_Monster) {
            Shape other = monster_shape(ent);
            Collision_Result res = collide_shape(shape, other);
            if (!res.collision) break;
            collide_push(res, &mon->pos, &ent->pos);
        }

        // Draw colliding box
        gfx_debug_box(eng->gfx_dbg, col->node->box, 1);
        if (ent->type == Entity_Wall) {
            m4 wall_inv = m4_invert_tr(ent->mtx);
            v3 p_local = m4_mul_pos(wall_inv, mon->pos);
            f32 rx = ent->size.x * .5;
            f32 ry = ent->size.y * .5;
            if (p_local.x < -rx) p_local.x = -rx;
            if (p_local.y < -ry) p_local.y = -ry;
            if (p_local.x > rx) p_local.x = rx;
            if (p_local.y > ry) p_local.y = ry;
            p_local.z = 0;

            v3 p_global = m4_mul_pos(ent->mtx, p_local);
            v3 dir = p_global - mon->pos;
            f32 dist = v3_length(dir);
            f32 pen = (box.max.x - box.min.x) * .5;
            // fmt_sf(OS_FMT, "D: ", dist, "\n");
            if (dist < pen) {
                mon->pos -= dir / dist * (pen - dist);
                // m4 hit = m4_id();
                // m4_translate(&hit, p_global);
                // gfx_debug_mtx(eng->gfx_dbg, hit);
            }
        }
    }
}

static void draw_shadow(Engine *eng, v3 shadow_pos, Image *image) {
    shadow_pos.y = SHADOW_OFFSET;

    m4 shadow_mtx = m4_id();
    m4_image_3d(&shadow_mtx, image);
    m4_rotate_x(&shadow_mtx, -R1);
    m4_translate(&shadow_mtx, shadow_pos);
    gfx_quad_3d(eng->gfx, shadow_mtx, image);
}

static void monster_update(Entity *mon, Entity *player, Image *gun, Sparse_Set *sparse, Engine *eng) {
    bool is_alive = mon->health > 0;

    entity_update_movement(mon, eng);

    if (is_alive) {
        monster_update_eyes(mon, eng);
        monster_update_ai(mon, player, eng);
        // monster_collide_with(mon, game->player);
        entity_collide(eng, sparse, mon);

        monster_wiggle(mon, eng);
        v3 dir = player->pos - mon->pos;
        mon->rot.y = f_atan2(dir.x, dir.z);
        mon->rot.z = R1 * f_sin2pi(mon->wiggle_phase) * mon->wiggle_amp * 0.25;
    } else {
        monster_die(mon, eng);
        mon->rot.x = -mon->death_animation * R1;
    }

    // Update matricies
    if (mon->type == Entity_Monster || mon->type == Entity_Player) {
        mon->mtx = m4_id();
        m4_rotate_z(&mon->mtx, mon->rot.z);
        m4_rotate_x(&mon->mtx, mon->rot.x);
        m4_rotate_y(&mon->mtx, mon->rot.y);
        m4_translate(&mon->mtx, mon->pos);

        mon->head_mtx = m4_id();
        m4_translate_y(&mon->head_mtx, mon->size.y * .7);
        m4_apply(&mon->head_mtx, mon->mtx);
        // gfx_draw_mtx(eng, mon->head_mtx);
    }

    if (mon->image) {
        m4 mtx = m4_id();
        m4_image_3d(&mtx, mon->image);
        m4_apply(&mtx, mon->mtx);
        m4_translate_y(&mtx, MONSTER_OFFSET);
        gfx_quad_3d(eng->gfx, mtx, mon->image);
        mon->image_mtx = mtx;
    }

    if (mon->shadow && is_alive) {
        draw_shadow(eng, mon->mtx.w, mon->shadow);
    }

    // Draw Gun
    {
        f32 aliveness = 1.0 - mon->death_animation;
        m4 mtx = m4_id();
        m4_image_3d(&mtx, gun);
        m4_translate_x(&mtx, -0.1 - 0.1 * mon->death_animation);
        m4_rotate_z(&mtx, -R1 * mon->death_animation * 0.2);
        m4_rotate_y(&mtx, R1 * .8 * aliveness);
        m4_translate_y(&mtx, (f32)(mon->sprite.image->size.y - mon->sprite.hand[0].y) / 32.0);
        m4_translate_x(&mtx, -(f32)mon->sprite.hand[0].x / 32.0 * 0.5f * 0.9);
        m4_apply(&mtx, mon->mtx);
        // gfx_draw_mtx(eng, mtx);
        gfx_quad_3d(eng->gfx, mtx, gun);
    }
}
