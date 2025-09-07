// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// ui.h - Immediate mode UI
#pragma once
#include "gfx/gfx.h"
#include "lib/str.h"
#include "lib/vec.h"
#include "qfn/ui_font.h"

typedef struct {
    v2 pos;
    v2 size;

    Input *input;
    Gfx *gfx;

    Memory *mem;

    Image *image;

    // NULL, A-Z, 0-9
    Image *chars[256];
} UI;

static UI *ui_new(Memory *mem, Input *input, Gfx *gfx) {
    UI *ui = mem_struct(mem, UI);
    ui->input = input;
    ui->gfx = gfx;
    ui->mem = mem;

    ui->image = image_new(mem, (v2u){16, 16});
    image_grid(ui->image, (v4){1, 0, 1, 1}, (v4){0, 0, 0, 1});
    return ui;
}

static void ui_begin(UI *ui) {
}

static Image *ui_font_render_cached(UI *ui, u8 chr) {
    if (ui->chars[chr]) return ui->chars[chr];
    Image *img = ui_font_render(ui->mem, chr, COLOR_BLACK);
    ui->chars[chr] = img;
    return img;
}

static void ui_text(UI *ui, m4 mtx, char *text) {
    f32 size = 24;

    f32 x = 0;
    f32 y = 0;
    for (;;) {
        char c = *text++;
        if (!c) break;

        if (c == '\n') {
            x = 0;
            y -= size;
            continue;
        }

        Image *img = ui_font_render_cached(ui, c);

        m4 mtx2 = m4_id();
        m4_scale(&mtx2, (v3){size, size, 1});
        m4_translate_x(&mtx2, x);
        m4_translate_y(&mtx2, y);
        m4_apply(&mtx2, mtx);
        gfx_draw_ui(ui->gfx, mtx2, img);

        x += size;
    }
}
