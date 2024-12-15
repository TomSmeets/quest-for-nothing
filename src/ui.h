#pragma once

#include "audio.h"
#include "gfx.h"
#include "vec.h"

typedef struct {
    v2 pos;
    v2 size;

    Audio *audio;
    Gfx *gfx;
} UI;

static void ui_begin(UI *ui) {
    m4 mtx = m4_id();
    gfx_quad_ui(ui->gfx, mtx, )
}
