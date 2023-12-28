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

static v2 rect_size(Rect *rect) {
    return rect->max - rect->min;
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


static bool rect_contains(Rect *r, v2 p) {
    return p.x >= r->min.x && p.x < r->max.x
        && p.y >= r->min.y && p.y < r->max.y;
}

static UI_Component ui_component(UI *ui, v2 offset, v2 size, f32 pad) {
    u32 id = ++ui->current_component_id;

    // NOTE: we start at a high 'y' and grow towards 0
    v2 pos = ui->row.max + offset;
    
    Rect outer = {
        .min = pos - (v2) { 0, size.y },
        .max = pos + (v2) { size.x, 0 },
    };

    Rect inner = {
        .min = outer.min + (v2) { pad, pad },
        .max = outer.max - (v2) { pad, pad },
    };


    bool hover = rect_contains(&inner, ui->input->mouse_pos);
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

    // grow size of the row
    if(outer.min.y < ui->row.min.y) ui->row.min.y = outer.min.y;
    if(outer.max.x > ui->row.max.x) ui->row.max.x = outer.max.x;
    if(outer.min.y < ui->window.min.y) ui->window.min.y = outer.min.y;
    if(outer.max.x > ui->window.max.x) ui->window.max.x = outer.max.x;

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
    ui->row.max = ui->row.min;
}

static void ui_begin(UI *ui, Input *input, mem *tmp) {
    ui->current_component_id = 0;
    ui->size = 50;
    ui->pad  = 4;

    ui->row.min.x = ui->window.min.x;
    ui->row.min.y = ui->window.max.y;
    ui->row.max = ui->row.min;

    ui->input = input;
    ui->gfx = gfx_begin(tmp);
    ui->gfx->mtx = m4_screen_to_clip(input->window_size);
    ui->gfx->depth = 0;

    if(ui->active && !input_is_down(ui->input, KEY_MOUSE_LEFT))
        ui->active = 0;

    gfx_rect(ui->gfx, ui->window.min, ui->window.max, WHITE);

    // title bar
    UI_Component comp = ui_component(ui, 0, (v2){rect_size(&ui->window).x, 20}, 0);
    UI_Component c2 = ui_component(ui, (v2){ -20, 0 }, (v2){20, 20}, 5);
    ui->window.max.x = ui->window.min.x;
    ui->window.min.y = ui->window.max.y;
    if(comp.down) {
        v2 window_pos = (v2){ui->window.min.x, ui->window.max.y};
        ui->window.min += input->mouse_pos - window_pos - ui->drag_offset;
        ui->window.max += input->mouse_pos - window_pos - ui->drag_offset;
    }
    gfx_rect(ui->gfx, comp.inner.min, comp.inner.max, WHITE);

    if(c2.click) {
    }
    gfx_rect(ui->gfx, c2.inner.min, c2.inner.max, WHITE);

    ui_newline(ui);
}

static void ui_end(UI *ui) {
    v2 error = { 0 };
    error.x += f_min(ui->row.min.x, 0);
    error.y += f_min(ui->row.min.y, 0);
    error.x += f_max(ui->row.max.x - ui->input->window_size.x, 0);
    error.y += f_max(ui->row.max.y - ui->input->window_size.y, 0);

    ui->window.min -= error*.5;
    ui->window.max -= error*.5;
}
