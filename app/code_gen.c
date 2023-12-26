// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// code_gen.c - Generate C code for this project
//
// The generated code should be code I would have written myself.
// Things this could do are:
// - Enum to String
// - Automatically add typedefs
// - ... that is all?
#include "fmt.h"
#include "os_generic.h"
#include "parse.h"

struct gen_enum {
    const char *name;
    const char *values;
};


// Return the pointer to the beginning of the next line, or null if we reach eof
char *next_line(char *l) {
    for(;;) {
        char c = *l;
        if(c == 0)    return l;
        if(c == '\n') return l+1;
        l++;
    }
}

static void fmt_keys(fmt_t *f) {
    for(u32 i = 0; i <= 9; ++i) {
        fmt_f(f, "KEY_%d,\n", i);
    }

    for(u32 i = 'A'; i <= 'Z'; ++i) {
        fmt_f(f, "KEY_%c,\n", i);
    }
}

static bool tok_is_op(Token *tok, char *op) {
    return tok->type == Token_Op && str_eq(tok->str, op);
}

static bool tok_is_symbol(Token *tok, char *sym) {
    return tok->type == Token_Symbol && str_eq(tok->str, sym);
}

static void handle_file(mem *m, char *path) {
    char *file = os_read_file(m, path).ptr;
    Token *tok = parse_token(m, file);
    assert(file);

    // for(Token *t = tok;t;t = t->next)
    //     os_printf("tok: %s\n", t->str);

    bool new_statement = 0;
    while(tok) {
        if(new_statement && tok_is_symbol(tok, "struct") && tok->next && tok->next->next && tok_is_op(tok->next->next, "{")) {
            tok = tok->next;
            char *name = tok->str;
            os_printf("typedef struct %s %s;\n", name, name);
        }

        if(new_statement && tok_is_symbol(tok, "enum")) {
            tok = tok->next;

            char *name = tok->str;
            os_printf("static const char *enum_to_str_%s[] = {\n", name);

            tok = tok->next; // {
            assert(tok_is_op(tok, "{"));

            while(tok && !tok_is_op(tok, "}")) {
                if(tok->type == Token_Symbol) {
                    os_printf("    \"%s\",\n", tok->str);
                }
                tok = tok->next;
            }
            os_printf("};\n");
        }

        new_statement = tok_is_op(tok, ";") || tok->type == Token_Macro;
        tok = tok->next;
    }
}

static void handle_dir(mem *m, char *dir) {
    for(os_dir *d = os_read_dir(m, dir); d; d = d->next) {
        if(!d->is_file) continue;
        char *path = fmt(m, "%s/%s", dir, d->file_name);
        os_printf("// ==== %s ====\n", path);
        handle_file(m, path);
    }
}

void *main_init(int argc, char *argv[]) {
    mem m = {};
    os_printf("#pragma once\n");
    handle_dir(&m, "src");
    handle_dir(&m, "app");
    return 0;
}

void main_update(void *handle) { os_exit(0); }
