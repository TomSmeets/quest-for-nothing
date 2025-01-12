// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// gfx_debug.h - Debug primitives for visualising vectors and matricies
#pragma once
#include "gfx.h"
#include "image.h"

typedef struct {
    // X,Y and Z colors
    Image *color[3];

    Gfx *gfx;
} Gfx_Debug;

static Gfx_Debug *gfx_debug_new(Gfx *gfx, Memory *mem) {
    Gfx_Debug *dbg = mem_struct(mem, Gfx_Debug);
    dbg->gfx = gfx;

    // Arrow size
    f32 sx = 4;
    f32 sy = 4;

    // X (Red), Y (Green), Z (Blue)
    for (u32 axis = 0; axis < 3; ++axis) {
        v4 color = { axis == 0, axis == 1, axis == 2, 1 };
        Image *img = image_new(mem, (v2u){sx, sy});
        for (u32 i = 0; i < sx * sy; ++i) img->pixels[i] = color;
        dbg->color[axis] = img;
    }
    return dbg;
}

static void gfx_debug_line(Gfx_Debug *dbg, f32 width, v3 pos, v3 dir, Image *img) {
    f32 len = v3_length(dir);

    v3 z = dir / len;
    v3 y = {0, 1, 0};
    v3 x = {1, 0, 0};

    if (dir.y * dir.y > dir.x * dir.x) {
        y = v3_cross(x, z);
        x = v3_cross(z, y);
    } else {
        x = v3_cross(z, y);
        y = v3_cross(x, z);
    }

    f32 arrow_size = 1.0f / 4;

    m4 mtx = { x, y, z, pos };
    for (u32 i = 0; i < 4; ++i) {
        {
            m4 mtx2 = m4_id();
            m4_rotate_x(&mtx2, R1);
            m4_translate_z(&mtx2, 0.5f);
            m4_scale(&mtx2, (v3){width, width, len});
            m4_rotate_z(&mtx2, R1 * i);
            m4_apply(&mtx2, mtx);
            gfx_quad_3d(dbg->gfx, mtx2, img);
        }
        {
            m4 mtx2 = m4_id();
            m4_rotate_x(&mtx2, R1);
            m4_translate_z(&mtx2, -0.44f);
            m4_scale(&mtx2, (v3){width, width, arrow_size});
            m4_rotate_y(&mtx2, R1*.5);
            m4_translate_z(&mtx2, len);
            m4_rotate_z(&mtx2, R1 * i);
            m4_apply(&mtx2, mtx);
            gfx_quad_3d(dbg->gfx, mtx2, img);
        }
        {
            m4 mtx2 = m4_id();
            m4_rotate_x(&mtx2, R1);
            m4_translate_z(&mtx2, -0.5f);
            m4_scale(&mtx2, (v3){width, width, arrow_size});
            m4_rotate_y(&mtx2, -R1*.5);
            m4_translate_z(&mtx2, len);
            m4_rotate_z(&mtx2, R1 * i);
            m4_apply(&mtx2, mtx);
            gfx_quad_3d(dbg->gfx, mtx2, img);
        }
        // {
        //     m4 mtx2 = m4_id();
        //     m4_translate_x(&mtx2, -0.5f);
        //     m4_rotate_y(&mtx2, -R1);
        //     m4_scale(&mtx2, (v3){width, width, width*10});
        //     m4_rotate_y(&mtx2, R1*.5);
        //     m4_translate_z(&mtx2, len);
        //     m4_rotate_z(&mtx2, R1 * i);
        //     m4_apply(&mtx2, mtx);
        //     gfx_quad_3d(dbg->gfx, mtx2, img);
        // }
        // {
        //     m4 mtx2 = m4_id();
        //     m4_translate_x(&mtx2, -0.5f);
        //     m4_rotate_y(&mtx2, -R1);
        //     m4_scale(&mtx2, (v3){width, width, width*10});
        //     m4_rotate_y(&mtx2, R1*.5);
        //     m4_translate_z(&mtx2, len);
        //     m4_rotate_z(&mtx2, R1 * i);
        //     m4_apply(&mtx2, mtx);
        //     gfx_quad_3d(dbg->gfx, mtx2, img);
        // }
    }
}

// Draw matrix
static void gfx_debug_mtx(Gfx_Debug *dbg, m4 mtx) {
    f32 width = 1.0f / 32.0f;
    gfx_debug_line(dbg, width, mtx.w, mtx.x, dbg->color[0]);
    gfx_debug_line(dbg, width, mtx.w, mtx.y, dbg->color[1]);
    gfx_debug_line(dbg, width, mtx.w, mtx.z, dbg->color[2]);
    // gfx_debug_line(dbg, 1, 0, mtx.w, dbg->image_arrow[2]);

    #if 0
    for (u32 axis = 0; axis < 3; ++axis) {
        for (u32 i = 0; i < 4; ++i) {
            m4 res = m4_id();
            m4_translate_x(&res, 0.5f);
            m4_scale(&res, (v3){1,.4,.4});
            m4_rotate_x(&res, R1 * i);
            if (axis == 1) m4_rotate_z(&res, R1);
            if (axis == 2) m4_rotate_y(&res, -R1);
            m4_apply(&res, mtx);
            gfx_quad_3d(dbg->gfx, res, dbg->image_arrow[axis]);
        }
    }
    #endif
}
