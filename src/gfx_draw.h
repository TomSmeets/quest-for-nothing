// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// gfx_draw.h - Draw 2d shapes
#pragma once
#include "gfx.h"
#include "str.h"

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

static void gfx_char(Gfx *g, v2 p, f32 sx, f32 sy, char c) {
    // upper case
    if(c >= 'a' && c <= 'z') c = c - 'a' + 'A';

    f32 b = f_min(sx, sy)*.125;
    f32 line_height = sy/3;
    f32 slope = sx / sy / 2;

    f32 y0 = 0;
    f32 y1 = sy*.5;
    f32 y2 = sy;
    f32 y3 = sy*.6;

    f32 x0 = 0;
    f32 x1 = sx*.5;
    f32 x2 = sx;

    f32 w = g->stroke_width;

    gfx_line_cap(g, 0, 0);
    if(c == 'A') {
        gfx_line_cap(g, 'x', 'x');
        gfx_line(g, p + (v2){x0, y2+w}, p + (v2){x1, -w});
        gfx_line(g, p + (v2){x2, y2+w}, p + (v2){x1, -w});
        gfx_line_cap(g, 0, 0);
        gfx_line(g,
            p + (v2){line_height*slope,      sy - line_height},
            p + (v2){sx - line_height*slope, sy - line_height}
        );
    }

    // Veritcal (L-L)
    if(str_chr("BDEFHKLMNPRW", c)) gfx_line(g, p + (v2){x0, y0-w}, p + (v2){x0,   y2+w});
    if(str_chr("IT", c))           gfx_line(g, p + (v2){x1, y0-w}, p + (v2){x1,   y2+w});
    if(str_chr("HMNW", c))         gfx_line(g, p + (v2){x2, y0-w}, p + (v2){x2,   y2+w});

    // Veritcal (S-S)
    if(str_chr("CGOQ", c))         gfx_line(g, p + (v2){x0, y0+b}, p + (v2){x0,   y2-b});
    if(str_chr("J", c))            gfx_line(g, p + (v2){x0, y1},   p + (v2){x0,   y2-b});
    if(str_chr("DOQ", c))          gfx_line(g, p + (v2){x2, y0+b}, p + (v2){x2,   y2-b});

    if(str_chr("BS", c))           gfx_line(g, p + (v2){x0, y0+b}, p + (v2){x0, y1-b});

    if(str_chr("BPR", c))          gfx_line(g, p + (v2){x2, y0+b}, p + (v2){x2, y1-b});
    if(str_chr("BS", c))           gfx_line(g, p + (v2){x2, y1+b}, p + (v2){x2, y2-b});

    // Veritcal (L-S)
    if(str_chr("U", c))            gfx_line(g, p + (v2){x0, y0-w}, p + (v2){x0,   y2-b});
    if(str_chr("JU", c))           gfx_line(g, p + (v2){x2, y0-w}, p + (v2){x2,   y2-b});

    // Horizontal (L-L)
    if(str_chr("EFIJTZ", c))       gfx_line(g, p + (v2){x0-w, y0}, p + (v2){x2+w, y0});
    if(str_chr("EFH", c))          gfx_line(g, p + (v2){x0-w, y1}, p + (v2){x2+w, y1});
    if(str_chr("EILZ", c))         gfx_line(g, p + (v2){x0-w, y2}, p + (v2){x2+w, y2});

    // Horizontal (L-S)
    if(str_chr("BDPR", c))         gfx_line(g, p + (v2){x0-w, y0}, p + (v2){x2-b, y0});
    if(str_chr("BPR", c))          gfx_line(g, p + (v2){x0-w, y1}, p + (v2){x2-b, y1});
    if(str_chr("BD", c))           gfx_line(g, p + (v2){x0-w, y2}, p + (v2){x2-b, y2});

    // Horizontal (S-S)
    if(str_chr("OQS", c))          gfx_line(g, p + (v2){x0+b, y0}, p + (v2){x2-b, y0});
    if(str_chr("S", c))            gfx_line(g, p + (v2){x0+b, y1}, p + (v2){x2-b, y1});
    if(str_chr("JOQSU", c))        gfx_line(g, p + (v2){x0+b, y2}, p + (v2){x2-b, y2});

    // Horizontal (S-L)
    if(str_chr("CG", c))           gfx_line(g, p + (v2){x0+b, y0}, p + (v2){x2+w, y0});
    if(str_chr("", c))             gfx_line(g, p + (v2){x0+b, y1}, p + (v2){x2+w, y1});
    if(str_chr("CG", c))           gfx_line(g, p + (v2){x0+b, y2}, p + (v2){x2+w, y2});

    // diagonal lines
    gfx_line_cap(g, 'X', 'Y');

    // Right
    if(str_chr("BDOPQRS", c))      gfx_line(g, p + (v2){x2-b, y0}, p + (v2){x2, y0+b});
    if(str_chr("BPR", c))          gfx_line(g, p + (v2){x2-b, y1}, p + (v2){x2, y1-b});
    if(str_chr("BS", c))           gfx_line(g, p + (v2){x2-b, y1}, p + (v2){x2, y1+b});
    if(str_chr("BDJOQSU", c))      gfx_line(g, p + (v2){x2-b, y2}, p + (v2){x2, y2-b});

    // Left
    if(str_chr("CGOQS", c))        gfx_line(g, p + (v2){x0+b, y0}, p +(v2){x0, y0+b});
    if(str_chr("S", c))            gfx_line(g, p + (v2){x0+b, y1}, p +(v2){x0, y1-b});
    if(str_chr("CGJOQSU", c))      gfx_line(g, p + (v2){x0+b, y2}, p +(v2){x0, y2-b});

    gfx_line_cap(g, 0, 0);

    // G
    if(c == 'G') {
        // extra corner
        gfx_line(g, p + (v2){x1, y1}, p + (v2){x2, y1});
        gfx_line(g, p + (v2){x2, y2}, p + (v2){x2, y1-w});
    }

    if(c == 'K') {
        f32 dy = sx*0.25;
        gfx_line_cap(g, 0, 'x');
        gfx_line(g, p + (v2){x0, y1+dy}, p + (v2){x2-w, y0-w});
        gfx_line(g, p + (v2){x0+dy, y1}, p + (v2){x2-w, y2+w});
        gfx_line_cap(g, 0, 0);
    }

    if(c == 'M') {
        gfx_line_cap(g, 'X', 'X');
        gfx_line(g, p + (v2){x0+w, y0}, p + (v2){x1-w*.5, sy*.6});
        gfx_line(g, p + (v2){x2-w, y0}, p + (v2){x1+w*.5, sy*.6});
        gfx_line_cap(g, 0, 0);
    }

    gfx_line_cap(g, 'X', 'X');
    if(c == 'N' || c == 'X') {
        gfx_line(g, p + (v2){x0+w, y0}, p + (v2){x2-w, y2});
    }

    if(c == 'X') gfx_line(g, p + (v2){x0+w, y2}, p + (v2){x2-w, y0});

    gfx_line_cap(g, 'x', 'x');
    if(c == 'Z') gfx_line(g, p + (v2){x0, y2-w}, p + (v2){x2, y0+w});

    if(c == 'Q') {
        gfx_line(g, p + (v2){x2-b-w, y2-b-w}, p + (v2){x2+w, y2+w});
    }

    if(c == 'R') {
        f32 dy = sx*.25;
        gfx_line_cap(g, 0, 'x');
        gfx_line(g, p + (v2){x0+dy, y1}, p + (v2){x2-w, y2+w});
        gfx_line_cap(g, 0, 0);
    }

    if(c == 'V') {
        gfx_line_cap(g, 'X', 'X');
        gfx_line(g, p + (v2){x0, y0}, p + (v2){x1-w*.5, y2});
        gfx_line(g, p + (v2){x2, y0}, p + (v2){x1+w*.5, y2});
        gfx_line_cap(g, 0, 0);
    }

    if(c == 'W') {
        gfx_line_cap(g, 'X', 'X');
        gfx_line(g, p + (v2){x0+w, y2}, p + (v2){x1-w*.5, sy*.4});
        gfx_line(g, p + (v2){x2-w, y2}, p + (v2){x1+w*.5, sy*.4});
        gfx_line_cap(g, 0, 0);
    }
}

static void gfx_text(Gfx *g, v2 start, f32 size) {
    size *= .8;
    f32 sx = size*.5;
    f32 sy = size;
    f32 pad = size*0.1;

    // width of things
    gfx_color(g, WHITE);
    gfx_stroke_width(g, size*.04);
    gfx_line_cap(g, 'c', 'c');

    char *text = "Hello World!\nABCDEFGHIJK\nLMNOPQRSTUV\nWXYZ\n0123456789";

    v2 p = start;
    for(;;) {
        char c = *text++;
        if(!c) break;
        p.x += pad;
        gfx_char(g, p, sx, sy, c);
        p.x += sx;
        p.x += pad;
        if(c == '\n') {
            p.x = start.x;
            p.y += sy + pad*2;
        }
    }
}
