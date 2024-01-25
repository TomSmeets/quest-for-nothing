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
#include "rect.h"
#include "gfx_draw.h"

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


static void ui_text(UI *ui, Rect *rect, char *text) {
    v2 center = rect_center(rect);
    f32 sy = rect_size(rect).y*.75;
    f32 sx = sy*1.2;
    v2 size = { str_len(text)*sx, sy };
    gfx_text(ui->gfx, center - size*.5, sx, sy, text);
}

static bool ui_button(UI *ui, char *text) {
    UI_Component comp = ui_component(ui, 0, (v2){ui->size*5, ui->size}, ui->pad);

    if(comp.click)
        os_printf("Clicked Button: %s\n", text);

    // Draw the component
    v4 color = WHITE;
    if(comp.hover) color = (v4){1, 0, 0, 1};
    if(comp.down)  color = (v4){0.4, 0, 0, 1};
    Rect smaller = comp.inner;
    rect_shrink_border(&smaller, 2);

    gfx_material(ui->gfx, BLACK, 0);
    gfx_rect(ui->gfx, comp.inner.min, comp.inner.max);

    gfx_material(ui->gfx, color, 0);
    gfx_rect(ui->gfx, smaller.min, smaller.max);

    gfx_material(ui->gfx, BLACK, 0);
    rect_shrink_border(&smaller, 8);
    ui_text(ui, &smaller, text);
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
    f32 title_height = 25;
    
    // Reset basic settings
    ui->current_component_id = 0;
    ui->size = 50;
    ui->pad  = 4;
    ui->input = input;

    // Start new render pass
    ui->gfx = gfx_begin(tmp);

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

    gfx_color(ui->gfx, (v4){.1, .1, .1, 1});
    gfx_rect(ui->gfx, ui->window.min, ui->window.min + window_size);

    // title bar
    UI_Component title_bar = ui_component(ui, 0, (v2){window_size.x, title_height},  0);

    gfx_color(ui->gfx, BLACK);
    gfx_rect(ui->gfx, title_bar.inner.min, title_bar.inner.max);

    if(title_bar.down) {
        v2 move_amount = -ui->drag_offset - title_bar.outer.min + input->mouse_pos;
        ui->window.min += move_amount;
        ui->window.max += move_amount;
        ui->row.min += move_amount;
        ui->row.max += move_amount;
    }

    gfx_material(ui->gfx, WHITE, 0);
    Rect text_rect = title_bar.inner;
    rect_shrink_border(&text_rect, 2);
    ui_text(ui, &text_rect, "Hello World!");

    // Start new line, and forget the min width
    ui_newline(ui);
    ui->window.max.x = ui->window.min.x;
}

static void ui_end(UI *ui) {
    m4_screen_to_clip(&ui->gfx->mtx, ui->input->window_size);
}
