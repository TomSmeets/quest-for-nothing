// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// gfx.h - Graphics API abstraction
#pragma once
#include "mem.h"
#include "math.h"
#include "mat.h"

// A good Graphics API should be
// - Simple and Flexible
// - Immediate mode?
// - Support 3d and 2d
static gfx_pass *gfx_pass_new(void);
static u32 gfx_put_vertex(gfx_pass *pass, v3 pos, v2 uv, v3 norm);
static void gfx_put_index(gfx_pass *pass, u32 ix);

// What is rendering? This is basically the api we want right?
// UI
// image *render_image(m4 *mtx, vtx *)

// NOTE: Should an image have a uuid? (u32/u64?)


// Textures
// What is the advantage of a texture atlas?

// 1. Entity
// 2. Quad + Image (optional)
// 3. Verts + Atlas
// 4. Upload To GPU

// 1. Entity
// 2. Verts


// For 3D graphics -> 8 floats
// For 2D graphics we would only need 4 floats:
//    struct gfx_vertex {
//        v2 pos;
//        v2 uv;
//    };
// But for simplicity and flexibility we will use the same
// vertex structure for both.
// We could use two variants in the future if we decide so.
struct gfx_vertex {
    v3 pos;
    v2 uv;
    v3 normal;
};


// graphics pass
// - shader
// - settings
// - transform
// - vertices
//
// Allocating the max number of vertices and indicies would use the following
// 65536*(8*4 + 2) = 2.12 MB
//
// u16 is a limit we could reach, so lets just use u32 for flexibility and simplicity
// 
struct gfx_pass {
    m4 transform;
    mem *m;

    // NOTE: We might need to split this up in the future
    u32 vtx_cap;
    u32 vtx_count;
    gfx_vertex *vtx;

    u32 idx_cap;
    u32 idx_count;
    u32 *idx;

    gfx_pass *next;
};

static gfx_pass *gfx_pass_new(mem *m, u32 vertex_count, u32 index_count) {
    gfx_pass *pass = mem_struct(m, gfx_pass);
    pass->vtx_cap = vertex_count;
    pass->vtx     = mem_array_uninit(m, gfx_vertex, pass->vtx_cap);
    pass->idx_cap = index_count;
    pass->idx     = mem_array_uninit(m, u32, pass->idx_cap);
    pass->m = m;
    return pass;
}

static u32 gfx_put_vtx(gfx_pass *pass, v3 pos, v2 uv, v3 normal) {
    assert(pass->vtx_count < pass->vtx_cap);
    u32 ix = pass->vtx_count++;
    pass->vtx[ix] = (gfx_vertex) {
        .pos = pos,
        .uv = uv,
        .normal = normal,
    };
    return ix;
};

static void gfx_put_idx(gfx_pass *pass, u32 idx) {
    assert(pass->idx_count < pass->idx_cap);
    pass->idx[pass->idx_count++] = idx;
}

// Texture Atlas
struct gfx_atlas_item {
    v2 uv_min;
    v2 uv_max;
};

static gfx_atlas_item gfx_put_image(gfx_pass *pass, image *img) {
    // TODO
    return (gfx_atlas_item) {};
}

// gfx_begin_pass
static void example_code(mem *m) {
    gfx_pass *pass = gfx_pass_new(m, model->vtx_count, model->idx_count);
    load_obj(read_file(obj));
}

// c---d
// | \ |
// a---b
static void gfx_put_quad(gfx_pass *pass, image *img, v3 a, v3 b, v3 c, v3 d) {
    v3 normal = { 0, 0, 1 };

    v4 uv = gfx_put_image(img);

    u32 ia = gfx_put_vtx(pass, a, (v2){0, 0}, normal);
    u32 ib = gfx_put_vtx(pass, b, (v2){1, 0}, normal);
    u32 ic = gfx_put_vtx(pass, c, (v2){0, 1}, normal);
    u32 id = gfx_put_vtx(pass, d, (v2){1, 1}, normal);

    gfx_put_idx(pass, ia);
    gfx_put_idx(pass, ib);
    gfx_put_idx(pass, ic);

    gfx_put_idx(pass, ic);
    gfx_put_idx(pass, ib);
    gfx_put_idx(pass, id);
}
