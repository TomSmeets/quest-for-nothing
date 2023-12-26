// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// ui.h - Immediate mode UI
#pragma once
#include "os.h"
#include "inc.h"
#include "vec.h"
#include "gfx.h"
#include "input.h"

// Simple grid like layout
struct UI {
    u32 index;
    u32 active;

    // size of one component (height)
    f32 size;
    f32 pad;

    // where we are currently drawing
    v2 pos;

    input *input;
};


static void ui_begin(UI *ui, input *input, mem *m) {
    ui->index = 1;
    ui->size = 80;
    ui->pad   = 4;
    ui->pos   = (v2) { 0, 0 };
    ui->input = input;

    if(ui->active && !input_is_down(ui->input, KEY_MOUSE_LEFT))
        ui->active = 0;
}

static gfx_pass *ui_end(UI *ui) {
    return 0;
}

static bool ui_button(UI *ui, const char *text) {
    u32 id = ui->index++;

    v2 outer_min = ui->pos;
    v2 outer_max = ui->pos + (v2) { 2 * ui->size, ui->size };

    v2 inner_min = outer_min + (v2) { ui->pad, ui->pad };
    v2 inner_max = outer_max - (v2) { ui->pad, ui->pad };

    bool hover = ui->input->mouse_pos.x >= inner_min.x && ui->input->mouse_pos.x < inner_max.x
              && ui->input->mouse_pos.y >= inner_min.y && ui->input->mouse_pos.y < inner_max.y;

    bool down  = hover && input_is_down(ui->input, KEY_MOUSE_LEFT);
    bool click = hover && input_is_click(ui->input, KEY_MOUSE_LEFT);

    if(click && !ui->active)
        ui->active = id;

    if(ui->active != id) {
        hover = 0;
        down = 0;
        click = 0;
    }

    if(click) {
        os_printf("Clicked Button: %s\n", text);
    }

    ui->pos.x = outer_max.x;
    return click;
}

static void ui_newline(UI *ui) {
    ui->pos.y += ui->size;
    ui->pos.x = 0;
}
