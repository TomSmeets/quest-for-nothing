// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gfx.h - Graphics, Audio, and Input handling abstraction implementation
#pragma once
#include "os_gfx.h"
#include "texture_packer.h"

typedef struct {
    OS_Gfx *os;
    Packer *pack;

    u32 quad_count;
    OS_Gfx_Quad quad_list[4096];

    u32 ui_quad_count;
    OS_Gfx_Quad ui_quad_list[4096];

    v2i viewport_size;
} Gfx;

static Gfx *gfx_init(Memory *mem, char *title) {
    Gfx *gfx = mem_struct(mem, Gfx);
    gfx->os = os_gfx_init(mem, title);
    gfx->pack = packer_new(OS_GFX_ATLAS_SIZE);
    return gfx;
}

static Input *gfx_begin(Gfx *gfx) {
    gfx->quad_count = 0;
    gfx->ui_quad_count = 0;

    // Recreate texture pack
    u32 cap = packer_capacity(gfx->pack, 32);
    if (cap < 16) {
        fmt_s(OS_FMT, "Debug: Recreating texture atlas\n");
        packer_free(gfx->pack);
        gfx->pack = packer_new(OS_GFX_ATLAS_SIZE);
    }

    Input *in = os_gfx_begin(gfx->os);
    gfx->viewport_size = in->window_size;
    return in;
}

static void gfx_end(Gfx *gfx, m4 camera) {
    m4 view = m4_invert_tr(camera);
    f32 aspect_x = gfx->viewport_size.x > gfx->viewport_size.y ? (f32)gfx->viewport_size.x / (f32)gfx->viewport_size.y : 1;
    f32 aspect_y = gfx->viewport_size.y > gfx->viewport_size.x ? (f32)gfx->viewport_size.y / (f32)gfx->viewport_size.x : 1;

    // 3d World
    m44 projection = m4_perspective_to_clip(view, 70, aspect_x, aspect_y, 0.1, 15.0);
    os_gfx_draw(gfx->os, projection, true, gfx->quad_count, gfx->ui_quad_list);
    gfx->quad_count = 0;

    // 2d UI
    m4 ui_view = m4_id();
    m44 ui_projection = m4_screen_to_clip(ui_view, gfx->viewport_size);
    os_gfx_draw(gfx->os, ui_projection, false, gfx->quad_count, gfx->ui_quad_list);
    gfx->ui_quad_count = 0;

    // Swap windows
    os_gfx_end(gfx->os);
}

static void gfx_quad(Gfx *gfx, m4 mtx, Image *img, bool ui) {
    if (!ui && gfx->quad_count >= array_count(gfx->quad_list)) {
        fmt_s(OS_FMT, "ERROR: Too many quads\n");
        return;
    }

    if (ui && gfx->ui_quad_count >= array_count(gfx->ui_quad_list)) {
        fmt_s(OS_FMT, "ERROR: Too many UI quads\n");
        return;
    }

    Packer_Area *area = packer_get_cache(gfx->pack, img);

    if (!area) {
        area = packer_get_new(gfx->pack, img);

        if (!area) {
            fmt_s(OS_FMT, "ERROR: Too many textures\n");
            return;
        }

        os_gfx_texture(gfx->os, area->pos, img);
    }

    OS_Gfx_Quad quad = {
        .x = {mtx.x.x, mtx.x.y, mtx.x.z},
        .y = {mtx.y.x, mtx.y.y, mtx.y.z},
        .z = {mtx.z.x, mtx.z.y, mtx.z.z},
        .w = {mtx.w.x, mtx.w.y, mtx.w.z},
        .uv_pos = {(f32)area->pos.x / OS_GFX_ATLAS_SIZE, (f32)area->pos.y / OS_GFX_ATLAS_SIZE},
        .uv_size = {(f32)img->size.x / OS_GFX_ATLAS_SIZE, (f32)img->size.y / OS_GFX_ATLAS_SIZE},
    };

    if (ui) {
        gfx->ui_quad_list[gfx->ui_quad_count++] = quad;
    } else {
        gfx->quad_list[gfx->quad_count++] = quad;
    }
}

static void gfx_quad_ui(Gfx *gfx, m4 mtx, Image *img) {
    gfx_quad(gfx, mtx, img, true);
}

static void gfx_quad_3d(Gfx *gfx, m4 mtx, Image *img) {
    gfx_quad(gfx, mtx, img, false);
}

static void gfx_set_grab(Gfx *gfx, bool grab) {
    os_gfx_set_grab(gfx->os, grab);
}
