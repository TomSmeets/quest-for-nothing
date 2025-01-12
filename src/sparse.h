// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sparse.h - Incremental BVH
#pragma once
#include "box.h"
#include "mem.h"

typedef struct Sparse Sparse;
typedef struct Sparse_Node Sparse_Node;
typedef struct Sparse_Cell Sparse_Cell;
typedef struct Sparse_Collision Sparse_Collision;

#define SPARSE_BOX_SIZE 4

struct Sparse_Collision {
    Sparse_Node *node;
    Sparse_Collision *next;
};

struct Sparse_Node {
    Box box;
    void *user;
    Sparse_Node *next;
};

// A big cell, contianing multiple nodes
struct Sparse_Cell {
    Box box;

    // Cell pos
    v3i pos;

    // Nodes in this Cell
    Sparse_Node *nodes;
    Sparse_Cell *next;
};

struct Sparse {
    Memory *mem;
    Sparse_Node *nodes;
    Sparse_Cell *cells;
};

static Sparse *sparse_new(Memory *mem) {
    Sparse *sparse = mem_struct(mem, Sparse);
    sparse->mem = mem;
    return sparse;
}

static void sparse_add(Sparse *sparse, Box box, void *user) {
    v3i pos = v3_to_v3i(box_center(box) / SPARSE_BOX_SIZE);

    // Find Cell at pos
    Sparse_Cell *cell = sparse->cells;
    for (;;) {
        if (!cell) break;
        if (v3i_eq(cell->pos, pos)) break;
        cell = cell->next;
    }

    if (!cell) {
        // No cell found, create a new one
        cell = mem_struct(sparse->mem, Sparse_Cell);
        cell->pos = pos;
        cell->box = box;
        cell->next = sparse->cells;
        sparse->cells = cell;
    } else {
        // Cell exists, grow box
        cell->box = box_union(cell->box, box);
    }

    // Add node to cell
    Sparse_Node *node = mem_struct(sparse->mem, Sparse_Node);
    node->box = box;
    node->user = user;
    node->next = cell->nodes;
    cell->nodes = node;
}

static Sparse_Collision *sparse_check(Sparse *sparse, Box box) {
    Sparse_Collision *col_list = 0;
    for (Sparse_Cell *cell = sparse->cells; cell; cell = cell->next) {
        if (!box_intersect(cell->box, box)) continue;
        for (Sparse_Node *node = cell->nodes; node; node = node->next) {
            if (!box_intersect(node->box, box)) continue;
            Sparse_Collision *col = mem_struct(sparse->mem, Sparse_Collision);
            col->next = col_list;
            col->node = node;
            col_list = col;
        }
    }
    return col_list;
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