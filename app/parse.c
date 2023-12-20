// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// parse.c - Some experiments with recursive decent parsers
#include "os_generic.h"
#include "fmt.h"

struct Parse {
    mem m;
    u8 *start;
    u8 *end;
};

struct Token {
    char *start;
    u32 len;

    u32 type;
    char *content;
};


static u8 *str_dup(mem *m, u8 *str, u32 len) {
    u8 *out = mem_push(m, len + 1);
    std_memcpy(out, str, len);
    out[len] = 0;
    return out;
}

static u8 parse_peek(Parse *p) {
    return *p->start;
}

static void parse_next(Parse *p) {
    p->start++;
}

static bool is_whitespace(u8 c) {
    if(c == ' ') return 1;
    if(c == '\n') return 1;
    if(c == '\t') return 1;
    if(c == '\r') return 1;
    return 0;
}

static void parse_space(Parse *p) {
    for(;;) {
        char c = parse_peek(p);
        if(!is_whitespace(c)) break;
        parse_next(p);
    }
}

static bool is_alpha(u8 c) {
    if (c >= 'a' && c <= 'z') return 1;
    if (c >= 'A' && c <= 'Z') return 1;
    if (c == '_') return 1;
    return 0;
}

static bool is_numeric(u8 c) {
    if (c >= '0' && c <= '9') return 1;
    return 0;
}

static bool is_alpha_numeric(u8 c) {
    return is_alpha(c) || is_numeric(c);
}

static u8 *parse_word(Parse *p) {
    parse_space(p);

    u8 *start = p->start;
    u8 first_char = parse_peek(p);
    assert(!is_whitespace(first_char));
    if(first_char == 0) return 0;
    parse_next(p);

    if(is_alpha_numeric(first_char)) {
        for(;;) {
            u8 c = parse_peek(p);
            if(!is_alpha_numeric(c)) break;
            parse_next(p);
        }
    } else if(first_char == '"' || first_char == '\'') {
        for(;;) {
            u8 c = parse_peek(p);
            if(c == 0) break;
            parse_next(p);
            if (c == '\\') {
                parse_next(p);
                continue;
            }
            if(c == first_char) break;
        }
    }
    return str_dup(&p->m, start, p->start - start);
}

void *main_init(int argc, char *argv[]) {
    mem m = {};
    buf file = os_read_file(&m, "src/input.h");
    Parse p = { .start = file.ptr, .end = file.ptr + file.size };

    u32 i = 0;
    for(;;) {
        u8 *line = parse_word(&p);
        if(!line) break;
        if(!str_eq((char *) line, "enum")) continue;

        u8 *name = parse_word(&p);
        assert(str_eq((char *) parse_word(&p), "{"));

        os_printf("const char to_string_enum_%s[] = { ", name);
        for(;;) {
            line = parse_word(&p);
            if(!line) break;
            if(*line == '{') continue;
            if(*line == ',') continue;
            if(*line == '}') break;
            os_printf("\"%s\", ", line);
        }
        os_printf("};\n");
    }

    mem_clear(&p.m);
    return 0;
}

void main_update(void *handle) { os_exit(0); }
