// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// ui.h - Immediate mode UI
#pragma once
#include "os.h"
#include "inc.h"
#include "vec.h"
#include "gfx.h"
#include "input.h"
#include "color.h"
#include "fmt.h"

struct Rect {
    v2 min;
    v2 max;
};

static void rect_shrink_border(Rect *rect, f32 amount) {
    rect->min += amount;
    rect->max -= amount;
}

static Rect rect_from_pos_size(v2 pos, v2 size) {
    return (Rect) { .min = pos, .max = pos + size };
}

static v2 rect_pos(Rect *rect) {
    return rect->min;
}

static v2 rect_top_right_corner(Rect *rect) {
    return (v2) { rect->max.x, rect->min.y };
}

static v2 rect_bottom_left_corner(Rect *rect) {
    return (v2) { rect->min.x, rect->max.y };
}

static v2 rect_size(Rect *rect) {
    return rect->max - rect->min;
}

static void rect_set_pos(Rect *rect, v2 pos) {
    v2 size = rect_size(rect);
    rect->min = pos;
    rect->max = pos + size;
}

static void rect_set_size(Rect *rect, v2 size) {
    rect->max = rect->min + size;
}

static bool rect_contains(Rect *r, v2 p) {
    return p.x >= r->min.x && p.x < r->max.x
        && p.y >= r->min.y && p.y < r->max.y;
}

// Simple grid like layout
struct UI {
    u32 current_component_id;

    // Component being held down
    u32 active;

    // Component hieght and padding
    f32 size;
    f32 pad;

    // where we are currently drawing
    Rect window;
    Rect row;

    Input *input;
    Gfx *gfx;
    // TODO: sound effects

    v2 drag_offset;
};

struct UI_Component {
    u32 id;
    bool hover;
    bool down;
    bool click;
    Rect inner;
    Rect outer;
};


static UI_Component ui_component(UI *ui, v2 offset, v2 size, f32 pad) {
    u32 id = ++ui->current_component_id;

    // NOTE: we start at a high 'y' and grow towards 0
    v2 pos = rect_top_right_corner(&ui->row) + offset;

    Rect outer = rect_from_pos_size(pos, size);
    Rect inner = outer;
    rect_shrink_border(&inner, pad);

    v2 mouse_pos = ui->input->mouse_pos;

    bool hover = rect_contains(&inner, mouse_pos);
    bool down  = hover && input_is_down(ui->input, KEY_MOUSE_LEFT);
    bool click = hover && input_is_click(ui->input, KEY_MOUSE_LEFT);

    if(click && !ui->active) {
        ui->active = id;
        ui->drag_offset = ui->input->mouse_pos - pos;
    }

    if(ui->active == id) {
        down = 1;
    } else {
        hover = hover && !ui->active;
        down  = 0;
        click = 0;
    }

    // grow size of the row and window
    if(outer.max.x > ui->row.max.x) ui->row.max.x = outer.max.x;
    if(outer.max.y > ui->row.max.y) ui->row.max.y = outer.max.y;

    if(outer.max.x > ui->window.max.x) ui->window.max.x = outer.max.x;
    if(outer.max.y > ui->window.max.y) ui->window.max.y = outer.max.y;

    return (UI_Component) {
        .inner = inner,
        .outer = outer,
        .hover = hover,
        .down  = down,
        .click = click,
    };
}



static bool ui_button(UI *ui, const char *text) {
    UI_Component comp = ui_component(ui, 0, (v2){ui->size*3, ui->size}, ui->pad);

    if(comp.click)
        os_printf("Clicked Button: %s\n", text);

    // Draw the component
    v4 color = WHITE;
    if(comp.hover) color = RED;
    if(comp.down)  color = BLUE;
    Rect smaller = comp.inner;
    rect_shrink_border(&smaller, 2);

    gfx_material(ui->gfx, WHITE, 0);
    gfx_rect(ui->gfx, comp.inner.min, comp.inner.max);

    gfx_material(ui->gfx, color, 0);
    gfx_rect(ui->gfx, smaller.min, smaller.max);

    // NOTE: Parameters, or pass function pointer?
    // I like the very direct function pointer method
    // snd_play(ui->snd, ui_button_sound);

    return comp.click;
}

static void ui_newline(UI *ui) {
    v2 pos = rect_bottom_left_corner(&ui->row);
    ui->row.min = pos;
    ui->row.max = pos;
}

static void ui_char(Gfx *g, v2 p, f32 sx, f32 sy, char c);
static void ui_text(Gfx *g, v2 start, f32 size) {
    size *= .8;
    f32 sx = size*.5;
    f32 sy = size;
    f32 pad = size*0.1;

    // width of things
    gfx_color(g, WHITE);
    gfx_stroke_width(g, size*.04);
    gfx_line_cap(g, 'c', 'c');

    char *text = "Hello World!\nABCDEFGHIJK\nLMNOPQRSTUV\nWXYZ";

    v2 p = start;
    for(;;) {
        char c = *text++;
        if(!c) break;
        p.x += pad;
        ui_char(g, p, sx, sy, c);
        p.x += sx;
        p.x += pad;
        if(c == '\n') {
            p.x = start.x;
            p.y += sy + pad*2;
        }
    }
}

static void ui_char(Gfx *g, v2 p, f32 sx, f32 sy, char c) {
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
    if(str_chr("IT", c))          gfx_line(g, p + (v2){x1, y0-w}, p + (v2){x1,   y2+w});
    if(str_chr("HMNW", c))        gfx_line(g, p + (v2){x2, y0-w}, p + (v2){x2,   y2+w});

    // Veritcal (S-S)
    if(str_chr("CGOQ", c))        gfx_line(g, p + (v2){x0, y0+b}, p + (v2){x0,   y2-b});
    if(str_chr("J", c))           gfx_line(g, p + (v2){x0, y1},   p + (v2){x0,   y2-b});
    if(str_chr("DOQ", c))         gfx_line(g, p + (v2){x2, y0+b}, p + (v2){x2,   y2-b});

    if(str_chr("BS", c))          gfx_line(g, p + (v2){x0, y0+b}, p + (v2){x0, y1-b});

    if(str_chr("BPR", c))         gfx_line(g, p + (v2){x2, y0+b}, p + (v2){x2, y1-b});
    if(str_chr("BS", c))          gfx_line(g, p + (v2){x2, y1+b}, p + (v2){x2, y2-b});

    // Veritcal (L-S)
    if(str_chr("U", c))           gfx_line(g, p + (v2){x0, y0-w}, p + (v2){x0,   y2-b});
    if(str_chr("JU", c))          gfx_line(g, p + (v2){x2, y0-w}, p + (v2){x2,   y2-b});

    // Horizontal (L-L)
    if(str_chr("EFIJTZ", c))       gfx_line(g, p + (v2){x0-w, y0}, p + (v2){x2+w, y0});
    if(str_chr("EFH", c))         gfx_line(g, p + (v2){x0-w, y1}, p + (v2){x2+w, y1});
    if(str_chr("EILZ", c))         gfx_line(g, p + (v2){x0-w, y2}, p + (v2){x2+w, y2});

    // Horizontal (L-S)
    if(str_chr("BDPR", c))        gfx_line(g, p + (v2){x0-w, y0}, p + (v2){x2-b, y0});
    if(str_chr("BPR", c))         gfx_line(g, p + (v2){x0-w, y1}, p + (v2){x2-b, y1});
    if(str_chr("BD", c))          gfx_line(g, p + (v2){x0-w, y2}, p + (v2){x2-b, y2});

    // Horizontal (S-S)
    if(str_chr("OQS", c))         gfx_line(g, p + (v2){x0+b, y0}, p + (v2){x2-b, y0});
    if(str_chr("S", c))           gfx_line(g, p + (v2){x0+b, y1}, p + (v2){x2-b, y1});
    if(str_chr("JOQSU", c))        gfx_line(g, p + (v2){x0+b, y2}, p + (v2){x2-b, y2});

    // Horizontal (S-L)
    if(str_chr("CG", c))          gfx_line(g, p + (v2){x0+b, y0}, p + (v2){x2+w, y0});
    if(str_chr("", c))            gfx_line(g, p + (v2){x0+b, y1}, p + (v2){x2+w, y1});
    if(str_chr("CG", c))          gfx_line(g, p + (v2){x0+b, y2}, p + (v2){x2+w, y2});

    // diagonal lines
    gfx_line_cap(g, 'X', 'Y');

    // Right
    if(str_chr("BDOPQRS", c))      gfx_line(g, p + (v2){x2-b, y0}, p + (v2){x2, y0+b});
    if(str_chr("BPR", c))         gfx_line(g, p + (v2){x2-b, y1}, p + (v2){x2, y1-b});
    if(str_chr("BS", c))           gfx_line(g, p + (v2){x2-b, y1}, p + (v2){x2, y1+b});
    if(str_chr("BDJOQSU", c))       gfx_line(g, p + (v2){x2-b, y2}, p + (v2){x2, y2-b});

    // Left
    if(str_chr("CGOQS", c))        gfx_line(g, p + (v2){x0+b, y0}, p +(v2){x0, y0+b});
    if(str_chr("S", c))            gfx_line(g, p + (v2){x0+b, y1}, p +(v2){x0, y1-b});
    if(str_chr("CGJOQSU", c))       gfx_line(g, p + (v2){x0+b, y2}, p +(v2){x0, y2-b});

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

static void ui_begin(UI *ui, Input *input, mem *tmp) {
    // Reset basic settings
    ui->current_component_id = 0;
    ui->size = 50;
    ui->pad  = 4;
    ui->input = input;

    // Start new render pass
    ui->gfx = gfx_begin(tmp);
    m4_screen_to_clip(&ui->gfx->mtx, input->window_size);

    // Draw previous window pos
    v2 window_size = rect_size(&ui->window);

    // Move the window to fit
    // v2 error = { 0 };
    // error.x += f_min(ui->row.min.x, 0);
    // error.y += f_min(ui->row.min.y, 0);
    // error.x += f_max(ui->row.max.x - ui->input->window_size.x, 0);
    // error.y += f_max(ui->row.max.y - ui->input->window_size.y, 0);
    // ui->window.min -= error;
    // ui->window.max -= error;

    // Move the window to 0,0
//    ui->window.min = (v2){};
    ui->window.max = ui->window.min;

    // Reset current row
    ui->row.min = ui->window.min;
    ui->row.max = ui->window.min;

    if(ui->active && !input_is_down(ui->input, KEY_MOUSE_LEFT))
        ui->active = 0;

    gfx_color(ui->gfx, (v4){1, 1, 1, .1});
    gfx_rect(ui->gfx, ui->window.min, ui->window.min + window_size);

    // Close Button
    UI_Component c2  = ui_component(ui, (v2){ 0, 0 }, (v2){20, 20}, 3);
    gfx_color(ui->gfx, WHITE);
    gfx_rect(ui->gfx, c2.inner.min, c2.inner.max);

    // title bar
    UI_Component title_bar = ui_component(ui, 0, (v2){window_size.x - 20, 20},  0);

    gfx_color(ui->gfx, WHITE);
    gfx_rect(ui->gfx, title_bar.inner.min, title_bar.inner.max);

    if(title_bar.down) {
        v2 move_amount = -ui->drag_offset - title_bar.outer.min + input->mouse_pos;
        ui->window.min += move_amount;
        ui->window.max += move_amount;
        ui->row.min += move_amount;
        ui->row.max += move_amount;
    }

    // Start new line, and forget the min width
    ui_newline(ui);
    ui->window.max.x = ui->window.min.x;
    ui_text(ui->gfx, (v2){100, 100}, 100);
}

static void ui_end(UI *ui) { }
