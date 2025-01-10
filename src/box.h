// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// box.h - AABB
#pragma once
#include "vec.h"

typedef struct {
    v3 min;
    v3 max;
} Box;

static bool box_intersect(Box a, Box b) {
    return (a.min.x < b.max.x && a.max.x > b.min.x) && //
           (a.min.y < b.max.y && a.max.y > b.min.y) && //
           (a.min.z < b.max.z && a.max.z > b.min.z);
}

static Box box_union(Box a, Box b) {
    if (a.min.x > b.min.x) a.min.x = b.min.x;
    if (a.min.y > b.min.y) a.min.y = b.min.y;
    if (a.min.z > b.min.z) a.min.z = b.min.z;

    if (a.max.x < b.max.x) a.max.x = b.max.x;
    if (a.max.y < b.max.y) a.max.y = b.max.y;
    if (a.max.z < b.max.z) a.max.z = b.max.z;
    return a;
}

static Box box_union_point(Box a, v3 p) {
    if (a.min.x > p.x) a.min.x = p.x;
    if (a.min.y > p.y) a.min.y = p.y;
    if (a.min.z > p.z) a.min.z = p.z;

    if (a.max.x < p.x) a.max.x = p.x;
    if (a.max.y < p.y) a.max.y = p.y;
    if (a.max.z < p.z) a.max.z = p.z;
    return a;
}
