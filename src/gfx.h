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

struct Gfx_Vertex {
    v3 pos;
    v2 uv;
    v3 normal;
};

// Graphics pass
struct Gfx {
    // Model to Screen transformation matrix
    m4 mtx;
    bool depth;

    u32 vertex_max;
    u32 vertex_count;
    Gfx_Vertex *vertex;

    u32 index_max;
    u32 index_count;
    u32 *index;

    Gfx *next;
};

static Gfx *gfx_begin(mem *m) {
    Gfx *gfx = mem_struct(m, Gfx);
    gfx->vertex_max = 1024;
    gfx->vertex = mem_array_uninit(m, Gfx_Vertex, gfx->vertex_max);

    gfx->index_max = 1024;
    gfx->index = mem_array_uninit(m, u32, gfx->index_max);
    return gfx;
}

static u32 gfx_vertex(Gfx *gfx, v3 pos, v2 uv, v3 normal) {
    assert(gfx->vertex_count < gfx->vertex_max);
    u32 ix = gfx->vertex_count++;
    gfx->vertex[ix] = (Gfx_Vertex) {
        .pos = pos,
        .uv = uv,
        .normal = normal,
    };
    return ix;
};

static void gfx_index(Gfx *gfx, u32 index) {
    assert(gfx->index_count < gfx->index_max);
    gfx->index[gfx->index_count++] = index;
}

// a---b
// | / |
// c---d
static void gfx_quad(Gfx *gfx, image *img, v3 normal, v3 a, v3 b, v3 c, v3 d) {
    u32 ia = gfx_vertex(gfx, a, (v2){0, 0}, normal);
    u32 ib = gfx_vertex(gfx, b, (v2){1, 0}, normal);
    u32 ic = gfx_vertex(gfx, c, (v2){0, 1}, normal);
    u32 id = gfx_vertex(gfx, d, (v2){1, 1}, normal);

    gfx_index(gfx, ia);
    gfx_index(gfx, ib);
    gfx_index(gfx, ic);

    gfx_index(gfx, ic);
    gfx_index(gfx, ib);
    gfx_index(gfx, id);
}

static void gfx_rect(Gfx *gfx, v2 min, v2 max, v4 color) {
    v3 normal = { 0, 0, 1 };
    v3 a = { min.x, min.y, 0 };
    v3 b = { max.x, min.y, 0 };
    v3 c = { min.x, max.y, 0 };
    v3 d = { max.x, max.y, 0 };
    gfx_quad(gfx, 0, normal, a, b, c, d);
}

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
//    struct Gfx_Vertex {
//        v2 pos;
//        v2 uv;
//    };
// But for simplicity and flexibility we will use the same
// vertex structure for both.
// We could use two variants in the future if we decide so.


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
    Gfx_Vertex *vtx;

    u32 idx_cap;
    u32 idx_count;
    u32 *idx;

    gfx_pass *next;
};

#if 0
static gfx_pass *gfx_pass_new(mem *m, u32 vertex_count, u32 index_count) {
    gfx_pass *pass = mem_struct(m, gfx_pass);
    pass->vtx_cap = vertex_count;
    pass->vtx     = mem_array_uninit(m, Gfx_Vertex, pass->vtx_cap);
    pass->idx_cap = index_count;
    pass->idx     = mem_array_uninit(m, u32, pass->idx_cap);
    pass->m = m;
    return pass;
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
#endif
