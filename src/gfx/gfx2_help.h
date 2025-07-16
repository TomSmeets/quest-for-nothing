#pragma once
#include "qfn/texture_packer.h"
#include "lib/mat.h"

typedef struct {
    f32 x[3];
    f32 y[3];
    f32 z[3];
    f32 w[3];
    f32 uv_pos[2];
    f32 uv_size[2];
} Gfx_Quad;

#define GFX_ATLAS_SIZE 4096

typedef struct Gfx_Pass Gfx_Pass;
struct Gfx_Pass {
    Image *img;
    m4 mtx;
    Gfx_Pass *next;
};

typedef struct {
    Memory *tmp;
    Gfx_Pass *pass_ui;
    Gfx_Pass *pass_3d;
    Packer *pack;
} Gfx_Helper;

static void gfx_help_begin(Gfx_Helper *help, Memory *tmp) {
    help->tmp = tmp;
    help->pass_3d = 0;
    help->pass_ui = 0;
}

typedef struct {
    v2u size;
    v2u pos;
    v4 *pixels;
} Gfx_Upload;

typedef struct {
    u32 upload_count;
    Gfx_Upload upload_list[1024];

    u32 quad_count;
    Gfx_Quad quad_list[1024];
} Gfx_Help_Fill_Result;


static Gfx_Quad gfx_help_make_quad(m4 mtx, v2u pos, v2u size) {
    return (Gfx_Quad){
        .x = {mtx.x.x, mtx.x.y, mtx.x.z},
        .y = {mtx.y.x, mtx.y.y, mtx.y.z},
        .z = {mtx.z.x, mtx.z.y, mtx.z.z},
        .w = {mtx.w.x, mtx.w.y, mtx.w.z},
        .uv_pos = {(f32)pos.x / GFX_ATLAS_SIZE, (f32)pos.y / GFX_ATLAS_SIZE},
        .uv_size = {(f32)size.x / GFX_ATLAS_SIZE, (f32)size.y / GFX_ATLAS_SIZE},
    };
}
static Gfx_Help_Fill_Result gfx_help_pull(Gfx_Help_Fill_Result *result, Gfx_Helper *help, Gfx_Pass **pass_list) {
    result->quad_count = 0;
    result->upload_count = 0;

    for(;;) {
        // Pull Item
        Gfx_Pass *pass = *pass_list;
        if(!pass) break;

        // Out of space for quads
        if(result->quad_count == array_count(result->quad_list)) break;

        // Check texture atlas for existing item
        Packer_Area *area = packer_get_cache(help->pack, pass->img);

        if(!area) {
            // Try to allocate space on the atlas
            area = packer_get_new(help->pack, pass->img);

            // No more space left
            if(!area) {
                packer_free(help->pack);
                help->pack = 0;
                break;
            }
        }

        // Insert Item
        result->quad_list[result->quad_count++] = gfx_help_make_quad(pass->mtx, area->pos, img->size);
        // =========================== CONTINUE HERE ===========================

        // Iterate to next item
        *pass_list = pass->next;
    }

    if (!help->pack) {
        help->pack = packer_new(GFX_ATLAS_SIZE);
    }

    // Check cache
    Packer_Area *area = packer_get_cache(help->pack, img);
    result->need_upload = false;
    if (!area) {
        area = packer_get_new(help->pack, img);
        if (!area) {
            packer_free(help->pack);
            help->pack = 0;
            return false;
        }
        result->need_upload = true;
        result->upload_pos = area->pos;
        result->upload_size = img->size;
        result->upload_pixels = img->pixels;
    }
    result->quad = gfx_help_make_quad(mtx, area->pos, img->size);
    return true;
}

static void gfx_help_push(Gfx_Helper *help, bool depth, m4 mtx, Image *img) {
    Gfx_Pass *pass = mem_struct(help->tmp, Gfx_Pass);
    pass->mtx = mtx;
    pass->img = img;
    if (depth) {
        pass->next = help->pass_3d;
        help->pass_3d = pass;
    } else {
        pass->next = help->pass_ui;
        help->pass_ui = pass;
    }
}

