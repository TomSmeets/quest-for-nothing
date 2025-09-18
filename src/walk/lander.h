#pragma once
#include "lib/std.h"
#include "lib/vec.h"

// One dimentional lunar lander
typedef struct {
    f32 old;
    f32 pos;
    f32 fuel;
    f32 score;
} Lander;

static void lander_step(Lander *lander, f32 dt, f32 input) {
    f32 acc = -9.81f;

    if (lander->fuel > 0) {
        acc += input * 15;
        lander->fuel -= input * dt;
    }

    f32 new = 2 * lander->pos - lander->old + acc *dt *dt;
    lander->old = lander->pos;
    lander->pos = new;

    if (lander->pos < 0) lander->pos = 0;

    if (lander->pos > lander->score) {
        lander->score = lander->pos;
    }
}
