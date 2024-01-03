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

    u32 vertex_max;
    u32 vertex_count;
    Gfx_Vertex *vertex;

    u32 index_max;
    u32 index_count;
    u32 *index;

    f32 stroke_width;
    Gfx_Vertex stamp;

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
    gfx->stamp.color    = color;
    gfx->stamp.emissive = emissive;
}

static void gfx_color(Gfx *gfx, v4 color) {
    gfx->stamp.color = color;
}

static void gfx_uv(Gfx *gfx, f32 u, f32 v) {
    gfx->stamp.uv.x = u;
    gfx->stamp.uv.y = v;
}

static void gfx_normal(Gfx *gfx, v3 normal) {
    gfx->stamp.normal = normal;
}

static u32 gfx_vertex(Gfx *gfx, v3 pos) {
    assert(gfx->vertex_count < gfx->vertex_max);
    u32 ix = gfx->vertex_count++;
    gfx->stamp.pos = pos;
    gfx->vertex[ix] = gfx->stamp;
    return ix;
};

static void gfx_stroke_width(Gfx *gfx, f32 w) {
    gfx->stroke_width = w;
}

static void gfx_index(Gfx *gfx, u32 index) {
    assert(gfx->index_count < gfx->index_max);
    gfx->index[gfx->index_count++] = index;
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

    gfx_index(gfx, ia);
    gfx_index(gfx, ic);
    gfx_index(gfx, ib);

    gfx_index(gfx, ic);
    gfx_index(gfx, id);
    gfx_index(gfx, ib);
}

static void gfx_border(Gfx *gfx, v2 min, v2 max) {
    f32 r = gfx->stroke_width;
    u32 iao = gfx_vertex(gfx, (v3) { min.x, min.y, 0});
    u32 ibo = gfx_vertex(gfx, (v3) { max.x, min.y, 0});
    u32 ico = gfx_vertex(gfx, (v3) { min.x, max.y, 0});
    u32 ido = gfx_vertex(gfx, (v3) { max.x, max.y, 0});

    u32 iai = gfx_vertex(gfx, (v3) { min.x + r, min.y + r, 0 });
    u32 ibi = gfx_vertex(gfx, (v3) { max.x - r, min.y + r, 0 });
    u32 ici = gfx_vertex(gfx, (v3) { min.x + r, max.y - r, 0 });
    u32 idi = gfx_vertex(gfx, (v3) { max.x - r, max.y - r, 0 });

    gfx_index(gfx, iao);
    gfx_index(gfx, ibi);
    gfx_index(gfx, ibo);
    gfx_index(gfx, iao);
    gfx_index(gfx, iai);
    gfx_index(gfx, ibi);

    gfx_index(gfx, ibo);
    gfx_index(gfx, idi);
    gfx_index(gfx, ido);
    gfx_index(gfx, ibo);
    gfx_index(gfx, ibi);
    gfx_index(gfx, idi);

    gfx_index(gfx, ido);
    gfx_index(gfx, ici);
    gfx_index(gfx, ico);
    gfx_index(gfx, ido);
    gfx_index(gfx, idi);
    gfx_index(gfx, ici);

    gfx_index(gfx, ico);
    gfx_index(gfx, iai);
    gfx_index(gfx, iao);
    gfx_index(gfx, ico);
    gfx_index(gfx, ici);
    gfx_index(gfx, iai);
}

static void gfx_rect(Gfx *gfx, v2 min, v2 max) {
    v3 a = { min.x, min.y, 0 };
    v3 b = { max.x, min.y, 0 };
    v3 c = { min.x, max.y, 0 };
    v3 d = { max.x, max.y, 0 };
    gfx_normal(gfx, (v3) { 0, 0, 1 });
    gfx_quad(gfx, a, b, c, d);
}

static void gfx_circle(Gfx *g, v2 p, f32 r) {
    u32 n = 8;
    f32 a = R4 / n;

    f32 r_cos = f_cos(a);
    f32 r_sin = f_sin(a);

    v2 rx = {  r_cos, r_sin };
    v2 ry = { -r_sin, r_cos };

    v2 d = (v2) { 1, 0 };

    u32 i_center = gfx_vertex(g, (v3){p.x, p.y});
    u32 i0       = 0;
    for(u32 i = 0; i <= n; ++i) {
        v2 o =  p + d*r;
        u32 i1 = gfx_vertex_uv(g, (v3){o.x, o.y, 0}, d.x*.5+.5, d.y*.5+.5);

        if(i > 0) {
            gfx_index(g, i_center);
            gfx_index(g, i0);
            gfx_index(g, i1);
        }
        d = rx * d.x + ry * d.y;
        i0 = i1;
    }
}

static void gfx_line(Gfx *g, v2 a, v2 b) {
    v2 d = v2_rot90(v2_normalize(a - b));
    f32 w = g->stroke_width;
    gfx_quad(g,
       (v3){a.x - d.x*w, a.y - d.y*w},
       (v3){a.x + d.x*w, a.y + d.y*w},
       (v3){b.x - d.x*w, b.y - d.y*w},
       (v3){b.x + d.x*w, b.y + d.y*w}
    );
    gfx_circle(g, a, w);
    gfx_circle(g, b, w);
}
