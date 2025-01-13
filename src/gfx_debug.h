// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// gfx_debug.h - Debug primitives for visualising vectors and matricies
#pragma once
#include "box.h"
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
        v4 color = {axis == 0, axis == 1, axis == 2, 1};
        Image *img = image_new(mem, (v2u){sx, sy});
        for (u32 i = 0; i < sx * sy; ++i) img->pixels[i] = color;
        dbg->color[axis] = img;
    }
    return dbg;
}

static void gfx_debug_line_ex(Gfx_Debug *dbg, v3 pos, v3 dir, u32 color, bool arrow) {
    assert(color < array_count(dbg->color), "Invalid color");
    Image *img = dbg->color[color];
    f32 len = v3_length(dir);
    f32 width = 1.0f / 32.0f;

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

    f32 arrow_size = 1.0f / 6;

    m4 mtx = {x, y, z, pos};
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
        if (arrow) {
            m4 mtx2 = m4_id();
            m4_rotate_x(&mtx2, R1);
            m4_translate_z(&mtx2, -0.44f);
            m4_scale(&mtx2, (v3){width, width, arrow_size});
            m4_rotate_y(&mtx2, R1 * .5);
            m4_translate_z(&mtx2, len);
            m4_rotate_z(&mtx2, R1 * i);
            m4_apply(&mtx2, mtx);
            gfx_quad_3d(dbg->gfx, mtx2, img);
        }
        if (arrow) {
            m4 mtx2 = m4_id();
            m4_rotate_x(&mtx2, R1);
            m4_translate_z(&mtx2, -0.5f);
            m4_scale(&mtx2, (v3){width, width, arrow_size});
            m4_rotate_y(&mtx2, -R1 * .5);
            m4_translate_z(&mtx2, len);
            m4_rotate_z(&mtx2, R1 * i);
            m4_apply(&mtx2, mtx);
            gfx_quad_3d(dbg->gfx, mtx2, img);
        }
    }
}

static void gfx_debug_line(Gfx_Debug *dbg, v3 p1, v3 p2, u32 color) {
    gfx_debug_line_ex(dbg, p1, p2 - p1, color, false);
}

static void gfx_debug_arrow(Gfx_Debug *dbg, v3 pos, v3 dir, u32 color) {
    gfx_debug_line_ex(dbg, pos, dir, color, true);
}

// Draw matrix
static void gfx_debug_mtx(Gfx_Debug *dbg, m4 mtx) {
    gfx_debug_line_ex(dbg, mtx.w, mtx.x, 0, true);
    gfx_debug_line_ex(dbg, mtx.w, mtx.y, 1, true);
    gfx_debug_line_ex(dbg, mtx.w, mtx.z, 2, true);
}

static void gfx_debug_box(Gfx_Debug *dbg, Box box, u32 color) {
    v3 sz = box_size(box);
    gfx_debug_line_ex(dbg, box.min, (v3){sz.x, 0, 0}, color, false);
    gfx_debug_line_ex(dbg, box.min, (v3){0, sz.y, 0}, color, false);
    gfx_debug_line_ex(dbg, box.min, (v3){0, 0, sz.z}, color, false);
    gfx_debug_line_ex(dbg, box.max, (v3){-sz.x, 0, 0}, color, false);
    gfx_debug_line_ex(dbg, box.max, (v3){0, -sz.y, 0}, color, false);
    gfx_debug_line_ex(dbg, box.max, (v3){0, 0, -sz.z}, color, false);
    gfx_debug_line_ex(dbg, box.min + (v3){sz.x, 0, 0}, (v3){0, 0, sz.z}, color, false);
    gfx_debug_line_ex(dbg, box.min + (v3){sz.x, 0, 0}, (v3){0, sz.y, 0}, color, false);
    gfx_debug_line_ex(dbg, box.min + (v3){0, sz.y, 0}, (v3){sz.x, 0, 0}, color, false);
    gfx_debug_line_ex(dbg, box.min + (v3){0, sz.y, 0}, (v3){0, 0, sz.z}, color, false);
    gfx_debug_line_ex(dbg, box.min + (v3){0, 0, sz.z}, (v3){sz.x, 0, 0}, color, false);
    gfx_debug_line_ex(dbg, box.min + (v3){0, 0, sz.z}, (v3){0, sz.y, 0}, color, false);
}
