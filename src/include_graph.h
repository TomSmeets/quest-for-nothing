#pragma once
#include "fmt.h"
#include "fs.h"
#include "math.h"
#include "mem.h"
#include "read.h"
#include "str_mem.h"

typedef struct Edge Edge;
typedef struct Graph Graph;
typedef struct Node Node;

struct Edge {
    Node *link;
    bool transitive;
    Edge *next;
};

struct Node {
    char *name;
    u32 size;
    Edge *edges;
    Node *next;
};

struct Graph {
    Memory *mem;
    Node *nodes;
};

static Node *graph_node(Graph *graph, char *name) {
    // Search
    for (Node *node = graph->nodes; node; node = node->next) {
        if (str_eq(node->name, name)) return node;
    }

    // Insert
    Node *node = mem_struct(graph->mem, Node);
    node->name = name;
    node->next = graph->nodes;
    graph->nodes = node;
    return node;
}

static void graph_link(Graph *graph, Node *src, Node *dst) {
    Edge *edge = mem_struct(graph->mem, Edge);
    edge->link = dst;
    edge->next = src->edges;
    src->edges = edge;
}

static void graph_fmt(Graph *graph, Fmt *fmt) {
    fmt_s(fmt, "digraph {\n");
    fmt_s(fmt, "  layout=dot;\n");
    fmt_s(fmt, "  node[style=filled,fillcolor=\"#ffffff\"];\n");
    fmt_s(fmt, "  edge[color=\"#bbbbbb\"];\n");
    for (Node *node = graph->nodes; node; node = node->next) {
        f32 size = (f32)f_sqrt(node->size) * 2;
        if (size < 14) size = 14;

        fmt_s(fmt, "  ");
        fmt_s(fmt, node->name);
        fmt_s(fmt, "[");
        fmt_sf(fmt, "fontsize=", size, "");
        fmt_s(fmt, "];\n");

        if (node->edges) {
            for (Edge *edge = node->edges; edge; edge = edge->next) {
                fmt_s(fmt, "  ");
                fmt_s(fmt, node->name);
                fmt_s(fmt, " -> ");
                fmt_s(fmt, edge->link->name);
                fmt_s(fmt, ";\n");
            }
        }
    }
    fmt_s(fmt, "}\n");
}

// A -> B -> C -> D
//  \____________/

// A->B
// A->D
// B->C
// C->D

// D(A,C)
// 1. Fill
// for (a,b) in graph
//     for n in dfs(b)
//         del(a,n)

// Generate Include Dot Graph
static void include_graph(void) {
    char *path = "src";

    Memory *mem = mem_new();

    Graph *graph = mem_struct(mem, Graph);
    graph->mem = mem;

    for (FS_Dir *file = fs_list(mem, path); file; file = file->next) {
        // Only .c and .h files
        bool is_h_file = str_ends_with(file->name, ".h");
        bool is_c_file = str_ends_with(file->name, ".c");
        if (!is_c_file && !is_h_file) continue;

        // Don't scan opengl api, it is quite big.
        if (str_eq(file->name, "ogl_api.h")) continue;

        // Full Path
        char *full_path = str_cat3(mem, path, "/", file->name);

        // Read file
        Read *read = read_new(mem, full_path);

        // Remove extention
        if (is_c_file || is_h_file) {
            file->name[str_len(file->name) - 2] = 0;
        }

        Node *node = graph_node(graph, file->name);

        u32 line_count = 0;
        for (;;) {
            char *buffer = mem_push_uninit(mem, 1024);
            char *line = read_line(read, buffer, 1024);
            if (!line) break;
            line_count++;

            char *prefix = "#include \"";
            char *suffix = "\"";
            if (!str_starts_with(line, prefix)) continue;
            if (!str_ends_with(line, suffix)) continue;

            u32 len = str_len(line);
            line[len - str_len(suffix)] = 0;
            line += str_len(prefix);

            // Ignore '../' paths
            if (line[0] == '.') continue;

            // Remove '.c' and '.h'
            if (str_ends_with(line, ".h") || str_ends_with(line, ".c")) {
                line[str_len(line) - 2] = 0;
            }
            Node *dst = graph_node(graph, line);
            graph_link(graph, node, dst);
        }
        node->size = line_count;
        read_close(read);
    }
    graph_fmt(graph, G->fmt);
    mem_free(mem);
}
