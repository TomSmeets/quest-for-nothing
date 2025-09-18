// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// verlet.h - Verlet softbody physics
#pragma once
#include "lib/fmt.h"
#include "lib/math.h"
#include "lib/mem.h"
#include "lib/std.h"
#include "lib/vec.h"

TYPEDEF_STRUCT(Vertex);
struct Vertex {
    v2 old;
    v2 pos;
    v2 vel;
    v2 grad;

    // Sensing
    f32 pressure;

    // List index
    Vertex *next;
};

static Vertex *vertex_new(Memory *mem, v2 pos) {
    Vertex *vtx = mem_struct(mem, Vertex);
    vtx->pos = pos;
    vtx->old = pos;
    return vtx;
}

static void vertex_step(Vertex *vtx, f32 dt) {
    v2 acc = {0, -9.81};
    v2 pos = vtx->pos;
    v2 old = vtx->old;
    v2 vel = pos - old;
    vel += vtx->grad * dt;
    v2 new = pos + vel + acc *dt *dt;
    vtx->grad = 0;
    vtx->old = pos;
    vtx->pos = new;
    vtx->vel = (new - pos) / dt;
    vtx->pressure = 0;
}

static void vertex_collide(Vertex *vtx) {
    if (vtx->pos.y < 0 && vtx->vel.y < 0) {
        vtx->pressure = 1;
        vtx->grad.y += -vtx->pos.y - vtx->vel.y;
        vtx->grad.x -= vtx->vel.x;
    }
}

TYPEDEF_STRUCT(Spring);
struct Spring {
    f32 len;
    Vertex *vtx_a;
    Vertex *vtx_b;

    // List index
    Spring *next;
};

static Spring *spring_new(Memory *mem, Vertex *xa, Vertex *xb) {
    Spring *spring = mem_struct(mem, Spring);
    spring->len = v2_length(xb->pos - xa->pos);
    spring->vtx_a = xa;
    spring->vtx_b = xb;
    return spring;
}

static void spring_step(Spring *spring) {
    v2 delta = spring->vtx_b->pos - spring->vtx_a->pos;
    f32 len = v2_length(delta);
    v2 dir = delta / len;

    f32 damp = v2_dot(dir, spring->vtx_a->vel - spring->vtx_b->vel);
    v2 grad = dir * ((spring->len - len) * 10 + damp * 0.5) * 0.5;
    spring->vtx_a->grad -= grad;
    spring->vtx_b->grad += grad;
}

TYPEDEF_STRUCT(Muscle);
struct Muscle {
    f32 angle;
    f32 angle_adjust;
    f32 angle_target;

    Vertex *vtx_a;
    Vertex *vtx_b;
    Vertex *vtx_c;

    // List index
    Muscle *next;
};

static Muscle *muscle_new(Memory *mem, Vertex *a, Vertex *b, Vertex *c) {
    Muscle *muscle = mem_struct(mem, Muscle);
    muscle->vtx_a = a;
    muscle->vtx_b = b;
    muscle->vtx_c = c;

    v2 va = v2_normalize(a->pos - b->pos);
    v2 vc = v2_normalize(c->pos - b->pos);
    muscle->angle = f_atan2(vc.y, vc.x) - f_atan2(va.y, va.x);
    return muscle;
}

static f32 f_angle_diff(f32 a, f32 b) {
    return f_wrap(a - b, -PI, PI);
}

static void muscle_step(Muscle *muscle, f32 dt) {
    v2 xa = muscle->vtx_a->pos;
    v2 xb = muscle->vtx_b->pos;
    v2 xc = muscle->vtx_c->pos;

    // Direction
    v2 da = xa - xb;
    v2 dc = xc - xb;

    // Distance
    v2 la = v2_length(da);
    v2 lc = v2_length(dc);

    // Normalized direction
    v2 na = da / la;
    v2 nc = dc / lc;

    // Tangent
    v2 ta = v2_rot90(na);
    v2 tc = v2_rot90(nc);

    // Angle: a -> c
    f32 angle = f_angle_diff(f_atan2(nc.y, nc.x), f_atan2(na.y, na.x));
    // f32 damp = v2_dot(ta, muscle->vtx_a->vel - muscle->vtx_b->vel) - v2_dot(tc, muscle->vtx_c->vel - muscle->vtx_b->vel);

    // Angle difference: target - real
    f32 adjust = f_angle_diff(muscle->angle + muscle->angle_target, angle);

    f32 damp_a = v2_dot(ta, muscle->vtx_a->vel - muscle->vtx_b->vel);
    f32 damp_c = v2_dot(tc, muscle->vtx_c->vel - muscle->vtx_b->vel);

    f32 adjust_a = adjust * 0.5;
    f32 adjust_c = -adjust * 0.5;

    // Grad
    v2 ga = -ta * (damp_a * 0.1 + adjust_a * 4) * (lc) / (la + lc);
    v2 gc = -tc * (damp_c * 0.1 + adjust_c * 4) * (la) / (la + lc);

    muscle->vtx_a->grad += ga;
    muscle->vtx_c->grad += gc;
    muscle->vtx_b->grad -= ga + gc;
}

TYPEDEF_STRUCT(Body);
struct Body {
    Memory *mem;
    Vertex *vtx;
    Spring *spring;
    Muscle *muscle;
};

static Body *body_new(Memory *mem) {
    Body *body = mem_struct(mem, Body);
    body->mem = mem;
    return body;
}

static Vertex *body_vertex(Body *body, v2 pos) {
    Vertex *vtx = vertex_new(body->mem, pos);
    LIST_PUSH(body->vtx, vtx, next);
    return vtx;
}

static Spring *body_spring(Body *body, Vertex *a, Vertex *b) {
    Spring *spring = spring_new(body->mem, a, b);
    LIST_PUSH(body->spring, spring, next);
    return spring;
}

static Muscle *body_muscle(Body *body, Vertex *a, Vertex *b, Vertex *c) {
    Muscle *muscle = muscle_new(body->mem, a, b, c);
    LIST_PUSH(body->muscle, muscle, next);
    return muscle;
}

static void verlet_step(Body *body, f32 dt) {
    for (Vertex *vtx = body->vtx; vtx; vtx = vtx->next) {
        vertex_step(vtx, dt);
        vertex_collide(vtx);
    }

    for (Spring *spring = body->spring; spring; spring = spring->next) {
        spring_step(spring);
    }

    for (Muscle *muscle = body->muscle; muscle; muscle = muscle->next) {
        muscle_step(muscle, dt);
    }
}
