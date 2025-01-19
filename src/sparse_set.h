// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sparse_set.h - Incremental BVH
#pragma once
#include "sparse.h"

typedef struct {
    Sparse *new;
    Sparse *old;
} Sparse_Set;

static Sparse_Set *sparse_set_new(Memory *mem) {
    Sparse_Set *set = mem_struct(mem, Sparse_Set);
    set->new = sparse_new(mem_new());
    set->old = sparse_new(mem_new());
    return set;
}

static void sparse_set_swap(Sparse_Set *set) {
    mem_free(set->old->mem);
    set->old = set->new;
    set->new = sparse_new(mem_new());
}

static void sparse_set_add(Sparse_Set *sparse, Box box, void *user) {
    sparse_add(sparse->new, box, user);
}

static Sparse_Collision *sparse_set_check(Sparse_Set *sparse, Box box) {
    return sparse_check(sparse->old, box);
}
