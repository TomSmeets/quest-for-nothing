// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// gfx_help.h - Helper methods for gfx implementations
#pragma once
#include "gfx/texture_packer.h"
#include "lib/mat.h"

#define GFX_ATLAS_SIZE 4096

typedef struct {
    f32 x[3];
    f32 y[3];
    f32 z[3];
    f32 w[3];
    f32 uv_pos[2];
    f32 uv_size[2];
} Gfx_Quad;

typedef struct Gfx_Pass Gfx_Pass;
struct Gfx_Pass {
    Image *img;
    m4 mtx;
    Gfx_Pass *next;
};

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
} Gfx_Pass_Compiled;

// Convert a matrix and texture region to a quad
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

// Insert quad into render pass
static void gfx_pass_push(Memory *mem, Gfx_Pass **pass_list, m4 mtx, Image *img) {
    Gfx_Pass *pass = mem_struct(mem, Gfx_Pass);
    pass->mtx = mtx;
    pass->img = img;
    pass->next = *pass_list;
    *pass_list = pass;
}

// Gather information on a draw pass
static bool gfx_pass_compile(Gfx_Pass_Compiled *result, Packer **pack, Gfx_Pass **pass_list) {
    if (!*pass_list) return false;

    // Reset result
    result->quad_count = 0;
    result->upload_count = 0;

    // Create texture packer if needed
    if (!*pack) {
        *pack = packer_new(GFX_ATLAS_SIZE);
    }

    for (;;) {
        // Pull Item
        Gfx_Pass *pass = *pass_list;
        if (!pass) break;

        // Out of space for quads
        if (result->quad_count == array_count(result->quad_list)) break;

        // Check texture atlas for existing item
        Packer_Area *area = packer_get_cache(*pack, pass->img);

        if (!area || area->variation != pass->img->variation) {
            // Out of space for texture uploads
            if (result->upload_count == array_count(result->upload_list)) break;

            // Try to allocate space on the atlas
            if (!area) {
                area = packer_get_new(*pack, pass->img);
            }

            // No more space left
            if (!area) {
                packer_free(*pack);
                *pack = 0;
                break;
            }

            result->upload_list[result->upload_count++] = (Gfx_Upload){
                .pos = area->pos,
                .size = pass->img->size,
                .pixels = pass->img->pixels,
            };
        }

        // Insert Item
        result->quad_list[result->quad_count++] = gfx_help_make_quad(pass->mtx, area->pos, pass->img->size);

        // Iterate to next item
        *pass_list = pass->next;
    }

    return true;
}
