// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// gfx_debug.h - Debug primitives for visualising vectors and matricies
#pragma once
#include "gfx.h"
#include "image.h"

typedef struct {
    // X,Y and Z arrows
    Image *image_arrow[3];

    Gfx *gfx;
} Gfx_Debug;

static Gfx_Debug *gfx_debug_new(Gfx *gfx, Memory *mem) {
    Gfx_Debug *dbg = mem_struct(mem, Gfx_Debug);
    dbg->gfx = gfx;

    // Arrow size
    f32 sx = 24;
    f32 sy = 12;

    // X (Red), Y (Green), Z (Blue)
    for (u32 axis = 0; axis < 3; ++axis) {
        v3 color = {
            axis == 0,
            axis == 1,
            axis == 2,
        };
        Image *img = image_new(mem, (v2u){sx, sy});
        img->origin = (v2u){0, sy / 2};
        for (u32 i = 0; i <= 3; ++i) {
            image_write(img, (v2i){sx - 1 - i, sy / 2 - i - 1}, color);
            image_write(img, (v2i){sx - 1 - i, sy / 2 + i}, color);
        }
        for (u32 x = 0; x < sx; ++x) {
            image_write(img, (v2i){x, sy / 2 - 1}, color);
            image_write(img, (v2i){x, sy / 2}, color);
        }
        dbg->image_arrow[axis] = img;
    }
    return dbg;
}

// Draw matrix
static void gfx_debug_mtx(Gfx_Debug *dbg, m4 mtx) {
    for (u32 axis = 0; axis < 3; ++axis) {
        for (u32 i = 0; i < 4; ++i) {
            m4 res = m4_id();
            m4_rotate_x(&res, R1 * i);
            if (axis == 1) m4_rotate_z(&res, R1);
            if (axis == 2) m4_rotate_y(&res, -R1);
            m4_apply(&res, mtx);
            gfx_quad_3d(dbg->gfx, res, dbg->image_arrow[axis]);
        }
    }
}
