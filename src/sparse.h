// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sparse.h - Sparse storage
#pragma once
#include "mem.h"
#include "vec.h"

typedef struct Sparse_Node {
    struct Sparse_Node *next;
    v3i pos;
    void *data;
} Sparse_Node;

typedef struct {
    Memory *mem;
    Sparse_Node *nodes[32 * 32];
} Sparse;

static Sparse *sparse_new(Memory *mem) {
    Sparse *sparse = mem_struct(mem, Sparse);
    sparse->mem = mem;
    return sparse;
}

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
