// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// ui.h - Immediate mode UI
#pragma once
#include "inc.h"
#include "vec.h"
#include "gfx.h"

// Simple grid like layout
struct UI {
    u32 index;
    gfx_pass *gfx;
    input *input;

    v2 component_size;
    v2 cursor;
};


static void ui_begin(UI *ui, input *input, mem *m) {
    ui->index = 0;
    ui->gfx = gfx_pass_new(m);
    ui->input = input;
}

static gfx_pass *ui_end(UI *ui) {
}


static void ui_button(UI *ui, const char *text) {
    v2 l = ui->cursor;
    v2 h = l + ui->component_size;
    ui->cursor.x = h.x;
}

static void ui_newline(UI *ui) {
    ui->cursor.x = 0;
    ui->cursor.y += ui->component_size.y;
}
