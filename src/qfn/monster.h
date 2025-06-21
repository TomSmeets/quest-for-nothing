// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// monster.h - Monster logic and AI
#pragma once
#include "lib/rand.h"
#include "lib/vec.h"
#include "qfn/collision.h"
#include "qfn/color.h"
#include "qfn/engine.h"
#include "qfn/entity.h"
#include "qfn/gfx.h"
#include "qfn/gfx_debug.h"
#include "qfn/image.h"
#include "qfn/monster_sprite.h"
#include "qfn/player.h"
#include "qfn/sparse_set.h"

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

#if 0
        // Draw colliding box
        // gfx_debug_box(eng->gfx_dbg, col->node->box, 1);
        if (ent->type == Entity_Wall) {
            // Monster collides with wall
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
            if (dist < pen) {
                mon->pos -= dir / dist * (pen - dist);
            }
        }
#endif
