// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gfx.h - Graphics, Audio, and Input handling abstraction implementation
#pragma once
#include "os_gfx.h"
#include "texture_packer.h"

typedef struct Gfx_Pass Gfx_Pass;
struct Gfx_Pass {
    u32 quad_count;
    OS_Gfx_Quad quad_list[2048];
    Gfx_Pass *next;
};

typedef struct {
    Gfx_Pass *first;
    Gfx_Pass *last;
} Gfx_Pass_List;

typedef struct {
    OS_Gfx *os;
    Packer *pack;
    v2i viewport_size;

    Memory *tmp;
    Gfx_Pass_List pass_ui;
    Gfx_Pass_List pass_3d;
} Gfx;

static Gfx *gfx_new(Memory *mem, char *title) {
    Gfx *gfx = mem_struct(mem, Gfx);
    gfx->os = os_gfx_init(mem, title);
    gfx->pack = packer_new(OS_GFX_ATLAS_SIZE);
    return gfx;
}

static Input *gfx_begin(Gfx *gfx) {
    gfx->pass_ui = (Gfx_Pass_List){0};
    gfx->pass_3d = (Gfx_Pass_List){0};
    gfx->tmp = mem_new();

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
    for (Gfx_Pass *pass = gfx->pass_3d.first; pass; pass = pass->next) {
        os_gfx_draw(gfx->os, projection, true, pass->quad_count, pass->quad_list);
    }

    // 2d UI
    m4 ui_view = m4_id();
    m44 ui_projection = m4_screen_to_clip(ui_view, gfx->viewport_size);
    for (Gfx_Pass *pass = gfx->pass_ui.first; pass; pass = pass->next) {
        os_gfx_draw(gfx->os, ui_projection, false, pass->quad_count, pass->quad_list);
    }

    // Swap windows
    os_gfx_end(gfx->os);
    mem_free(gfx->tmp);
}

static OS_Gfx_Quad *gfx_pass_quad(Gfx *gfx, Gfx_Pass_List *pass_list) {
    Gfx_Pass *pass = pass_list->last;

    // If the pass is full or not exiting, creat a new one
    if (!pass || pass->quad_count == array_count(pass->quad_list)) {
        pass = mem_struct(gfx->tmp, Gfx_Pass);
        LIST_APPEND(pass_list->first, pass_list->last, pass);
    }

    u32 i = pass->quad_count++;
    return pass->quad_list + i;
}

static void gfx_quad(Gfx *gfx, m4 mtx, Image *img, bool ui) {
    Packer_Area *area = packer_get_cache(gfx->pack, img);

    if (!area) {
        area = packer_get_new(gfx->pack, img);

        if (!area) {
            fmt_s(OS_FMT, "ERROR: Too many textures\n");
            return;
        }

        os_gfx_texture(gfx->os, area->pos, img);
    } else if (area->variation != img->variation) {
        area->variation = img->variation;
        os_gfx_texture(gfx->os, area->pos, img);
    }

    // Scale to image size
    // mtx.w.x -= (f32) img->origin.x - (f32) img->size.x*.5;
    // mtx.w.y -= (f32) img->origin.y - (f32) img->size.y*.5;
    // mtx.x *= img->size.x / 32.0f;
    // mtx.y *= img->size.y / 32.0f;

    f32 pixel_scale = ui ? 4.0f : 1.0f / 32.0f;
    m4 mtx2 = m4_id();
    m4_scale(&mtx2, (v3){img->size.x, img->size.y, 1});
    m4_translate_x(&mtx2, (f32)img->size.x / 2.0f - (f32)img->origin.x);
    m4_translate_y(&mtx2, (f32)img->origin.y - (f32)img->size.y / 2.0f);
    m4_scale(&mtx2, (v3){pixel_scale, pixel_scale, 1});
    m4_apply(&mtx2, mtx);

    *gfx_pass_quad(gfx, ui ? &gfx->pass_ui : &gfx->pass_3d) = (OS_Gfx_Quad){
        .x = {mtx2.x.x, mtx2.x.y, mtx2.x.z},
        .y = {mtx2.y.x, mtx2.y.y, mtx2.y.z},
        .z = {mtx2.z.x, mtx2.z.y, mtx2.z.z},
        .w = {mtx2.w.x, mtx2.w.y, mtx2.w.z},
        .uv_pos = {(f32)area->pos.x / OS_GFX_ATLAS_SIZE, (f32)area->pos.y / OS_GFX_ATLAS_SIZE},
        .uv_size = {(f32)img->size.x / OS_GFX_ATLAS_SIZE, (f32)img->size.y / OS_GFX_ATLAS_SIZE},
    };
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

static void gfx_set_fullscreen(Gfx *gfx, bool full) {
    os_gfx_set_fullscreen(gfx->os, full);
}
