// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// todotreesrv.c - Generic Tree storage database
#include "os_generic.h"
#include "fmt.h"

struct hello {
    mem mem;
    u32 counter;
    u64 start_time;
    u64 time;
    Global global;
};


struct Header {
    u64 version;
    u64 next_id;
    Node *root;
    Node *yank;
    mem *mem;
};

struct Node {
    u64 id;
    u64 state;
    char *text;
    Node *child;
    Node *next;

    Header *hdr;
};

// id child next name state

static void put_u64(fmt_t *f, u64 var) {
    fmt_buf(f, sizeof(var), (u8*) &var);
}

static void put_u32(fmt_t *f, u32 var) {
    fmt_buf(f, sizeof(var), (u8*) &var);
}

static void put_str(fmt_t *f, char *str) {
    fmt_buf(f, str_len(str) + 1, (u8*) str);
}

static void *get_raw(Buffer *buf, u64 size) {
    assert(buf->size >= size);
    void *ptr = buf->ptr;
    buf->size -= size;
    buf->ptr  += size;
    return ptr;
}

static u64 get_u64(Buffer *buf) {
    return *(u64 *) get_raw(buf, sizeof(u64));
}

static char *get_str(Buffer *buf) {
    char *ptr = (char *) buf->ptr;
    u32 len = str_len(ptr) + 1;
    assert(buf->size >= len);
    buf->ptr += len;
    buf->size -= len;
    return ptr;
}

static void put_node(fmt_t *f, Node *n) {
    if(!n) {
        put_u64(f, 0);
        return;
    }

    put_u64(f, n->id);
    put_u64(f, n->state);
    put_str(f, n->text);
    put_node(f, n->child);
    put_node(f, n->next);
}


static Node *get_node(mem *m, Buffer *buf, Header *hdr) {
    u64 id = get_u64(buf);
    if(!id) return 0;
    
    Node *n = mem_struct(m, Node);
    n->id = id;
    n->state = get_u64(buf);
    n->text  = get_str(buf);
    n->child = get_node(m, buf, hdr);
    n->next = get_node(m, buf, hdr);
    n->hdr = hdr;
    return n;
}

static void put_header(fmt_t *f, Header *h) {
    put_u64(f, h->version);
    put_u64(f, h->next_id);
    put_node(f, h->root);
    put_node(f, h->yank);
}

static Header *get_header(mem *m, Buffer *b) {
    Header *h = mem_struct(m, Header);
    h->version = get_u64(b);
    h->next_id = get_u64(b);
    h->root    = get_node(m, b, h);
    h->yank    = get_node(m, b, h);
    h->mem = m;
    return h;
}

static Node *node_new_dangling(Header *hdr, char *text) {
    Node *node = mem_struct(hdr->mem, Node);
    node->hdr  = hdr;
    node->id   = hdr->next_id++;
    node->text = text;
    return node;
}

static Node *node_new(Node *parent, char *text) {
    Header *hdr = parent->hdr;
    Node *node = mem_struct(hdr->mem, Node);
    node->hdr  = hdr;
    node->id   = hdr->next_id++;
    node->text = text;

    if(!parent->child) {
        parent->child = node;
    } else {
        for(Node *prev = parent->child; prev; prev = prev->next) {
            if(prev->next) continue;
            prev->next = node;
            break;
        }
    }
    return node;
}

static mem *mem_new(void) {
    mem m_stack = {};
    mem *m = mem_struct(&m_stack, mem);
    *m = m_stack;
    return m;
}

static Header *header_get(void) {
    Header *hdr = 0;
    mem *m = mem_new();
    Buffer out = os_read_file(m, "todo.bin");
    if(out.ptr) {
        os_print("Header found!\n");
        hdr = get_header(m, &out);
    } else {
        os_print("File not found, Creating new header..\n");
        hdr = mem_struct(m, Header);
        hdr->version = 1;
        hdr->next_id = 1;
        hdr->mem     = m;
        hdr->root    = node_new_dangling(hdr, "root");
        hdr->yank    = node_new_dangling(hdr, "yank");
    }
    return hdr;
}

static void header_save(Header *hdr) {
    fmt_t f = { .mem = hdr->mem };
    put_header(&f, hdr);

    Buffer buf = { .ptr = f.buffer, .size = f.used };
    os_write_file("todo.bin", buf);
    os_print("Saved header\n");
}

static void node_print(fmt_t *f, u32 indent, Node *n) {
    for(u32 i = 0; i < indent; ++i) {
        fmt_str(f, "  ");
    }
    fmt_u64(f, n->id, 10, 2, 0, 0);
    fmt_str(f, " ");
    fmt_str(f, n->text);
    fmt_str(f, "\n");
    if(n->child) node_print(f, indent + 1, n->child);
    if(n->next)  node_print(f, indent,     n->next);
}

void *main_init(int argc, char *argv[]) {
    os_printf("main_init called\n");
    mem *tmp = mem_new();

    Header *hdr = header_get();

    // Node *p = node_new(hdr->root, "Hallo Ik ben Tom");
    // Node *c0 = node_new(p, "Child 1");
    // Node *c1 = node_new(p, "Child 2");

    fmt_t f = { .mem = tmp };
    node_print(&f, 0, hdr->root);
    node_print(&f, 0, hdr->yank);
    os_print(fmt_end(&f));

    header_save(hdr);

    return hdr;
}

void main_update(void *handle) {
    os_exit(0);
}
