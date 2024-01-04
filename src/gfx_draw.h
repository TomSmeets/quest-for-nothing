// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// gfx_draw.h - Draw 2d shapes
#pragma once
#include "gfx.h"

static void gfx_stroke_width(Gfx *gfx, f32 w) {
    gfx->stroke_width = w;
}

static void gfx_line_cap(Gfx *gfx, u8 start, u8 end) {
    gfx->cap_start = start;
    gfx->cap_end = end;
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

    gfx_triangle(gfx, iao, ibi, ibo);
    gfx_triangle(gfx, iao, iai, ibi);

    gfx_triangle(gfx, ibo, idi, ido);
    gfx_triangle(gfx, ibo, ibi, idi);

    gfx_triangle(gfx, ido, ici, ico);
    gfx_triangle(gfx, ido, idi, ici);

    gfx_triangle(gfx, ico, iai, iao);
    gfx_triangle(gfx, ico, ici, iai);
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
            gfx_triangle(g, i_center, i0, i1);
        }
        d = rx * d.x + ry * d.y;
        i0 = i1;
    }
}

static void gfx_line(Gfx *g, v2 a, v2 b) {
    v2 d = v2_normalize(b - a);
    v2 n = v2_rot90(d);
    f32 w = g->stroke_width;


    v2 q1 = a - w*n;
    v2 q2 = a + w*n;
    v2 q3 = b - w*n;
    v2 q4 = b + w*n;

    if(g->cap_start == 'b') {
        q1 -= w*d;
        q2 -= w*d;
    }

    if(g->cap_end == 'b') {
        q3 += w*d;
        q4 += w*d;
    }

    {
        v2 f1 = w * d * (d.x/d.y);
        v2 f2 = w * (d / f_abs(d.y));

        if(g->cap_start == 'x') {
            q1 +=  f1;
            q2 += -f1;
        }

        if(g->cap_end == 'x') {
            q3 +=  f1;
            q4 += -f1;
        }

        if(g->cap_start == 'X') {
            q1 +=  f1 - f2;
            q2 += -f1 - f2;
        }

        if(g->cap_end == 'X') {
            q3 +=  f1 + f2;
            q4 += -f1 + f2;
        }
    }

    {
        v2 f1 = w * d * (d.y/d.x);
        v2 f2 = w * (d / f_abs(d.x));

        if(g->cap_start == 'y') {
            q1 += -f1;
            q2 += +f1;
        }

        if(g->cap_end == 'y') {
            q3 += -f1;
            q4 += +f1;
        }

        if(g->cap_start == 'Y') {
            q1 += -f1 - f2;
            q2 += +f1 - f2;
        }

        if(g->cap_end == 'Y') {
            q3 += -f1 + f2;
            q4 += +f1 + f2;
        }
    }

    v3 p1 = {q1.x, q1.y};
    v3 p2 = {q2.x, q2.y};
    v3 p3 = {q3.x, q3.y};
    v3 p4 = {q4.x, q4.y};

    gfx_quad(g, p1, p2, p3, p4);

    if(g->cap_start == 'c') gfx_circle(g, a, w);
    if(g->cap_end   == 'c') gfx_circle(g, b, w);
}
