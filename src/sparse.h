// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sparse.h - Incremental BVH
#pragma once
#include "box.h"
#include "mem.h"

typedef struct Sparse Sparse;
typedef struct Sparse_Node Sparse_Node;
typedef struct {
    Sparse_Node *node;
} Sparse_Collision;

static Sparse *sparse_new(Memory *mem);
static void sparse_add(Sparse *sparse, Box box, void *user);

static void *sparse_check(Sparse *sparse, Box box, Sparse_Collision *result);

// ============================================================

struct Sparse_Node {
    Box box;
    void *user;
    Sparse_Node *next;
};

struct Sparse {
    Memory *mem;
    Sparse_Node *nodes;
};

static Sparse *sparse_new(Memory *mem) {
    Sparse *sparse = mem_struct(mem, Sparse);
    sparse->mem = mem;
    return sparse;
}

static void sparse_add(Sparse *sparse, Box box, void *user) {
    Sparse_Node *node = mem_struct(sparse->mem, Sparse_Node);
    node->box = box;
    node->user = user;
    node->next = sparse->nodes;
    sparse->nodes = node;
}

static void *sparse_check(Sparse *sparse, Box box, Sparse_Collision *iter) {
    Sparse_Node *node = sparse->nodes;

    // Continue if called multiple times
    if (iter->node) node = iter->node->next;

    for (;;) {
        // No more found
        if (!node) return 0;

        // Found intersecting node
        if (box_intersect(node->box, box)) {
            iter->node = node;
            return node->user;
        }

        node = node->next;
    }
}

#if 0
static u32 sparse_hash(v3i pos) {
    return (u32)(pos.x % 32) + (u32)(pos.z % 32) * 32;
}

static void sparse_put(Sparse *sparse, v3i pos, void *data) {
    u32 key = sparse_hash(pos);
    Sparse_Node *next = sparse->nodes[key];
    Sparse_Node *node = mem_struct(sparse->mem, Sparse_Node);
    node->next = next;
    node->data = data;
    node->pos = pos;
    sparse->nodes[key] = node;
}

static Sparse_Node *sparse_find(Sparse *sparse, Sparse_Node *node, v3i pos) {
    if (!node) {
        // First time
        u32 key = sparse_hash(pos);
        node = sparse->nodes[key];
    } else {
        node = node->next;
    }
    for (;;) {
        // No more nodes left
        if (!node) return 0;

        // Found the node
        if (v3i_eq(node->pos, pos)) return node;

        // Keep searching
        node = node->next;
    }
    return node;
}
#endif
