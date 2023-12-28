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
    if(comp.down) color = BLUE;
    if(comp.click) color = GREEN;
    gfx_rect(ui->gfx, comp.inner.min, comp.inner.max, color);

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

static void ui_begin(UI *ui, Input *input, mem *tmp) {
    // Reset basic settings
    ui->current_component_id = 0;
    ui->size = 50;
    ui->pad  = 4;
    ui->input = input;

    // Start new render pass
    ui->gfx = gfx_begin(tmp);
    ui->gfx->mtx = m4_screen_to_clip(input->window_size);

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

    gfx_rect(ui->gfx, ui->window.min, ui->window.min + window_size, WHITE);

    // Close Button
    UI_Component c2  = ui_component(ui, (v2){ 0, 0 }, (v2){20, 20}, 3);
    gfx_rect(ui->gfx, c2.inner.min, c2.inner.max, WHITE);

    // title bar
    UI_Component title_bar = ui_component(ui, 0, (v2){window_size.x - 20, 20},  0);
    gfx_rect(ui->gfx, title_bar.inner.min, title_bar.inner.max, WHITE);

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
}

static void ui_end(UI *ui) { }
