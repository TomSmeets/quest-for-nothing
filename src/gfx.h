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
    v4 color;
    f32 emissive;
};

// Graphics pass
struct Gfx {
    // Model to Screen transformation matrix
    m4 mtx;
    bool depth;

    // Verticies
    u32 vertex_max;
    u32 vertex_count;
    Gfx_Vertex *vertex;

    // Indicies into the vertex array
    // Allows for reuse of verticies
    u32 index_max;
    u32 index_count;
    u32 *index;

    // A statefull api to keep the number of function arguments small
    Gfx_Vertex next_vertex;

    // 2D drawing state

    // Width of lines and borders
    f32 stroke_width;

    // End cap type of lines
    // - b    Extend by stroke_width
    // - c    Circle
    // - x/y  Align to the x/y axis
    // - X/Y  Align to the x/y axis, extended by stroke_width
    u8 cap_end;
    u8 cap_start;

    // Not used yet
    // Possible use to chain mutiple passes
    Gfx *next;
};

static Gfx *gfx_begin(mem *m) {
    Gfx *gfx = mem_struct(m, Gfx);
    gfx->vertex_max = 1024*10;
    gfx->vertex = mem_array_uninit(m, Gfx_Vertex, gfx->vertex_max);

    gfx->index_max = 1024*10;
    gfx->index = mem_array_uninit(m, u32, gfx->index_max);

    gfx->mtx = m4_id();
    return gfx;
}


static void gfx_material(Gfx *gfx, v4 color, f32 emissive) {
    gfx->next_vertex.color    = color;
    gfx->next_vertex.emissive = emissive;
}

static void gfx_color(Gfx *gfx, v4 color) {
    gfx->next_vertex.color = color;
}

static void gfx_uv(Gfx *gfx, f32 u, f32 v) {
    gfx->next_vertex.uv.x = u;
    gfx->next_vertex.uv.y = v;
}

static void gfx_normal(Gfx *gfx, v3 normal) {
    gfx->next_vertex.normal = normal;
}

// Submit a vertex to the vertex array with the current material/uv and normal state
static u32 gfx_vertex(Gfx *gfx, v3 pos) {
    // TODO: when full, dynamically grow, either the array or create a new pass
    assert(gfx->vertex_count < gfx->vertex_max);
    u32 ix = gfx->vertex_count++;
    gfx->next_vertex.pos = pos;
    gfx->vertex[ix] = gfx->next_vertex;
    return ix;
}

static void gfx_index(Gfx *gfx, u32 index) {
    assert(gfx->index_count < gfx->index_max);
    gfx->index[gfx->index_count++] = index;
}

static void gfx_triangle(Gfx *gfx, u32 a, u32 b, u32 c) {
    gfx_index(gfx, a);
    gfx_index(gfx, b);
    gfx_index(gfx, c);
}

static u32 gfx_vertex_uv(Gfx *gfx, v3 pos, f32 u, f32 v) {
    gfx_uv(gfx, u, v);
    return gfx_vertex(gfx, pos);
};


// a---b
// | / |
// c---d
static void gfx_quad(Gfx *gfx, v3 a, v3 b, v3 c, v3 d) {
    u32 ia = gfx_vertex_uv(gfx, a, 0, 0);
    u32 ib = gfx_vertex_uv(gfx, b, 1, 0);
    u32 ic = gfx_vertex_uv(gfx, c, 0, 1);
    u32 id = gfx_vertex_uv(gfx, d, 1, 1);

    gfx_triangle(gfx, ia, ic, ib);
    gfx_triangle(gfx, ic, id, ib);
}
