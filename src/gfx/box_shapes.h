#pragma once
#include "gfx/box.h"
#include "lib/mat.h"

static Box box_from_quad(m4 mtx) {
    v3 p0 = m4_mul_pos(mtx, (v3){-0.5, -0.5, 0});
    v3 p1 = m4_mul_pos(mtx, (v3){+0.5, -0.5, 0});
    v3 p2 = m4_mul_pos(mtx, (v3){-0.5, +0.5, 0});
    v3 p3 = m4_mul_pos(mtx, (v3){+0.5, +0.5, 0});

    Box box = {p0, p0};
    box = box_union_point(box, p1);
    box = box_union_point(box, p2);
    box = box_union_point(box, p3);
    return box;
}

static Box box_from_cylinder(v3 pos, f32 height, f32 radius) {
    return (Box){
        .min = pos + (v3){-radius, 0, -radius},
        .max = pos + (v3){radius, height, radius},
    };
}
