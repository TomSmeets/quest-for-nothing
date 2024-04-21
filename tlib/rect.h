// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// rect.h - Axis aligned 2d rectangle
#pragma once
#include "tlib/vec.h"

struct Rect {
    v2 min;
    v2 max;
};

static void rect_shrink_border(Rect *rect, f32 amount) {
    rect->min += amount;
    rect->max -= amount;
}

static Rect rect_from_pos_size(v2 pos, v2 size) {
    return (Rect){.min = pos, .max = pos + size};
}

static v2 rect_pos(Rect *rect) {
    return rect->min;
}

static v2 rect_center(Rect *rect) {
    return 0.5 * rect->min + 0.5 * rect->max;
}

static v2 rect_top_right_corner(Rect *rect) {
    return (v2){rect->max.x, rect->min.y};
}

static v2 rect_bottom_left_corner(Rect *rect) {
    return (v2){rect->min.x, rect->max.y};
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
    return p.x >= r->min.x && p.x < r->max.x && p.y >= r->min.y && p.y < r->max.y;
}
