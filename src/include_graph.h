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
    u32 rank;
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

    {
        u32 max_rank = 0;
        for (Node *node = graph->nodes; node; node = node->next) {
            if (max_rank < node->rank) max_rank = node->rank;
        }

        for (u32 i = 0; i < max_rank + 1; ++i) {
            fmt_s(fmt, "  { rank=same;");

            for (Node *node = graph->nodes; node; node = node->next) {
                if (node->rank != i) continue;
                fmt_s(fmt, node->name);
                fmt_s(fmt, "; ");
            }

            fmt_s(fmt, "}\n");
        }
    }

    for (Node *node = graph->nodes; node; node = node->next) {
        fmt_s(fmt, "  ");
        fmt_s(fmt, node->name);
        fmt_s(fmt, "[");
        fmt_s(fmt, "];\n");

        if (node->edges) {
            for (Edge *edge = node->edges; edge; edge = edge->next) {
                if (edge->transitive) continue;

                fmt_s(fmt, "  ");
                fmt_s(fmt, node->name);
                fmt_s(fmt, " -> ");
                fmt_s(fmt, edge->link->name);

                if (edge->transitive) {
                    fmt_s(fmt, "[");
                    fmt_s(fmt, "constraint=false,style=dotted");
                    fmt_s(fmt, "]");
                };
                fmt_s(fmt, ";\n");
            }
        }
    }
    fmt_s(fmt, "}\n");
}

// Transitive Reduction:
//
// for (a, b) in edges
//   for n in dfs(b)
//       del(a,n)

// Mark link as transitive
static void graph_unlink(Node *src, Node *dst) {
    for (Edge *edge = src->edges; edge; edge = edge->next) {
        if (edge->link == dst) edge->transitive = true;
    }
}

static void graph_tred_dfs(Node *src, Node *node) {
    for (Edge *edge = node->edges; edge; edge = edge->next) {
        Node *dst = edge->link;
        graph_unlink(src, dst);

        if (!edge->transitive) graph_tred_dfs(src, dst);
    }
}

static void graph_tred(Graph *graph) {
    for (Node *node = graph->nodes; node; node = node->next) {
        for (Edge *edge = node->edges; edge; edge = edge->next) {
            if (edge->transitive) continue;
            graph_tred_dfs(node, edge->link);
        }
    }
}

static void graph_rank(Graph *graph) {
    for (;;) {
        bool changed = false;
        for (Node *node = graph->nodes; node; node = node->next) {
            for (Edge *edge = node->edges; edge; edge = edge->next) {
                Node *other = edge->link;
                u32 rank = other->rank + 1;
                if (node->rank < rank) {
                    node->rank = rank;
                    changed = true;
                }
            }
        }
        if (!changed) break;
    }
}

static void graph_read_file(Graph *graph, char *path, char *name) {
    // Read file
    Read *read = read_new(graph->mem, path);
    Node *node = graph_node(graph, name);

    u32 line_count = 0;
    for (;;) {
        char *buffer = mem_push_uninit(graph->mem, 1024);
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

static void graph_read_dir(Graph *graph, char *path) {
    for (FS_Dir *file = fs_list(graph->mem, path); file; file = file->next) {
        if (file->is_dir) continue;

        // Only .c and .h files
        bool is_h_file = str_ends_with(file->name, ".h");
        bool is_c_file = str_ends_with(file->name, ".c");
        if (!is_c_file && !is_h_file) continue;

        // Don't scan opengl api, it is quite big.
        if (str_eq(file->name, "ogl_api.h")) continue;

        // Full Path
        char *full_path = str_cat3(graph->mem, path, "/", file->name);

        // Remove extention
        if (is_c_file || is_h_file) {
            file->name[str_len(file->name) - 2] = 0;
        }

        graph_read_file(graph, full_path, file->name);
    }
}

// Generate Include Dot Graph
static void include_graph(void) {
    Memory *mem = mem_new();
    Graph *graph = mem_struct(mem, Graph);
    graph->mem = mem;
    graph_read_dir(graph, "src");
    graph_tred(graph);
    graph_rank(graph);
    graph_fmt(graph, G->fmt);
    mem_free(mem);
}
