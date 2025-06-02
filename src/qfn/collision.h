// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// collision.h - 3d Collision Math
#pragma once
#include "lib/vec.h"
#include "qfn/box.h"
#include "qfn/mat.h"

// Centered quad facing +z
typedef struct {
    // Orientation
    m4 mtx;

    // Quad radius
    v2 radius;
} Quad;

// Cylinder facying y axis
typedef struct {
    // Center position
    v3 pos;

    // Cylinder radius
    v2 radius;
} Cylinder;

typedef struct {
    enum {
        Shape_None,
        Shape_Cylinder,
        Shape_Quad,
    } type;

    union {
        Cylinder cylinder;
        Quad quad;
    };
} Shape;

// Supported shapes
// Box
// Capsule (y=up)
// Quad

// Construct a bounding box around a quad
static Box box_from_quad(Quad quad) {
    v3 p0 = m4_mul_pos(quad.mtx, (v3){-quad.radius.x, -quad.radius.y, 0});
    v3 p1 = m4_mul_pos(quad.mtx, (v3){+quad.radius.x, -quad.radius.y, 0});
    v3 p2 = m4_mul_pos(quad.mtx, (v3){-quad.radius.x, +quad.radius.y, 0});
    v3 p3 = m4_mul_pos(quad.mtx, (v3){+quad.radius.x, +quad.radius.y, 0});

    Box box = {p0, p0};
    box = box_union_point(box, p1);
    box = box_union_point(box, p2);
    box = box_union_point(box, p3);
    return box;
}

static Box box_from_quad2(m4 mtx) {
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

// Create a bounding box around a y-aligned cylinder
// pos is the bottom center of the cylinder.
// size is { diameter, height}
static Box box_from_cylinder(Cylinder cyl) {
    return (Box){
        .min = cyl.pos - (v3){cyl.radius.x, cyl.radius.y, cyl.radius.x},
        .max = cyl.pos + (v3){cyl.radius.x, cyl.radius.y, cyl.radius.x},
    };
}

static Box box_from_shape(Shape shape) {
    if (shape.type == Shape_Cylinder) return box_from_cylinder(shape.cylinder);
    if (shape.type == Shape_Quad) return box_from_quad(shape.quad);
    assert(false, "Impossible Shape");
    return (Box){};
}

typedef struct {
    bool collision;

    // Collision point in A
    v3 pos_a;

    // Collision point in B
    v3 pos_b;
} Collision_Result;

static Collision_Result collide_cyl_cyl(Cylinder a, Cylinder b) {
    Collision_Result result = {};

    // Check collision in y axis
    f32 y_a_min = a.pos.y - a.radius.y;
    f32 y_a_max = a.pos.y + a.radius.y;

    f32 y_b_min = b.pos.y - b.radius.y;
    f32 y_b_max = b.pos.y + b.radius.y;

    // a -> b in xz plane
    v3 dir = b.pos - a.pos;
    dir.y = 0;

    // No collision
    if (y_a_max < y_b_min || y_b_max < y_a_min) return result;

    // Check xz plane
    f32 distance_sq = v3_length_sq(dir);
    f32 max_dist = a.radius.x + b.radius.x;

    // No collision
    if (distance_sq > max_dist * max_dist) return result;

    f32 distance = f_sqrt(distance_sq);
    v3 dir_norm = dir / distance;

    v3 pos_a = a.pos + dir_norm * a.radius.x;
    v3 pos_b = b.pos - dir_norm * b.radius.x;
    v3 hit = (pos_a + pos_b) / 2;

    // if(y_a_max - y_b_min < 0.1) {
    //     // B on top of A
    //     // pos_a = hit;
    //     // pos_b = hit;
    //     pos_a.y = y_a_max;
    //     pos_b.y = y_b_min;
    // } else if(y_b_max - y_a_min < 0.1) {
    //     // A on top of B
    //     // pos_a = hit;
    //     pos_b = hit;
    //     pos_a.y = y_a_min;
    //     pos_b.y = y_b_max;
    // } else {
    // Collision in xz plane
    f32 y = (pos_a.y + pos_b.y) / 2;
    pos_a.y = y;
    pos_b.y = y;
    // }

    result.collision = true;
    result.pos_a = pos_a;
    result.pos_b = pos_b;
    return result;
}

static Collision_Result collide_cyl_quad(Cylinder a, Quad b) {
    // TODO
    Collision_Result res = {};
    return res;
}

// Swap collision direction
static Collision_Result collision_swap(Collision_Result collision) {
    Collision_Result r2 = collision;
    r2.pos_a = collision.pos_b;
    r2.pos_b = collision.pos_a;
    return r2;
}

static Collision_Result collide_shape(Shape a, Shape b) {
    Collision_Result result = {};
    //   C Q
    // C x x
    // Q . -

    // Ensure A.type <= B.type
    if (a.type > b.type) {
        result = collide_shape(b, a);
        result = collision_swap(result);
        return result;
    }

    if (a.type == Shape_Cylinder) {
        if (b.type == Shape_Cylinder) {
            result = collide_cyl_cyl(a.cylinder, b.cylinder);
        }

        if (b.type == Shape_Quad) {
            result = collide_cyl_quad(a.cylinder, b.quad);
        }
    }

    return result;
}

// Apply push force based on the collision result
static void collide_push(Collision_Result res, v3 *a, v3 *b) {
    if (!res.collision) return;

    // From A -> B
    v3 dir = (res.pos_b - res.pos_a) * 0.5f;
    // dir = v3_limit(dir, 0, 0.05f);
    *a += dir * .5 * .1;
    *b -= dir * .5 * .1;
}
