// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// collision.h - 3d Collision Math
#pragma once
#include "box.h"
#include "mat.h"
#include "vec.h"

// Construct a bounding box around a quad
static Box box_from_quad(m4 mtx, v2 size) {
    v3 p0 = m4_mul_pos(mtx, (v3){-0.5 * size.x, -0.5 * size.y, 0});
    v3 p1 = m4_mul_pos(mtx, (v3){0.5 * size.x, -0.5 * size.y, 0});
    v3 p2 = m4_mul_pos(mtx, (v3){-0.5 * size.x, 0.5 * size.y, 0});
    v3 p3 = m4_mul_pos(mtx, (v3){0.5 * size.x, 0.5 * size.y, 0});

    Box box = {p0, p0};
    box = box_union_point(box, p1);
    box = box_union_point(box, p2);
    box = box_union_point(box, p3);
    return box;
}

// Create a bounding box around a y-aligned cylinder
// pos is the bottom center of the cylinder.
// size is { diameter, height}
static Box box_from_cylinder(v3 pos, v2 size) {
    f32 h = size.y;
    f32 r = size.x / 2;
    return (Box){
        .min = pos - (v3){r, 0, r},
        .max = pos + (v3){r, h, r},
    };
}
