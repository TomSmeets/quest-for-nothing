// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// parse.h - A text parser
#pragma once
#include "tlib/fmt.h"
#include "tlib/inc.h"
#include "tlib/mem.h"
#include "tlib/str.h"

// ==== Simple Checks ===
static bool is_alpha(char c) {
    if (c >= 'a' && c <= 'z')
        return 1;
    if (c >= 'A' && c <= 'Z')
        return 1;
    if (c == '_')
        return 1;
    return 0;
}

static bool is_digit(char c) {
    if (c >= '0' && c <= '9')
        return 1;
    return 0;
}

static bool is_whitespace(char c) {
    if (c == ' ')
        return 1;
    if (c == '\n')
        return 1;
    if (c == '\t')
        return 1;
    if (c == '\r')
        return 1;
    return 0;
}

static bool is_eol(char c) {
    return c == 0 || c == '\n';
}

static bool is_alpha_numeric(char c) {
    return is_alpha(c) || is_digit(c);
}

static bool is_non_space(char c) {
    return c && !is_whitespace(c);
}

// ==== Recursive Decent Parser With Backtracking ====
struct Parse {
    char *start;
    char *end;
    char *cursor;
    bool error;
    u32 line;
};

static Parse parse_new(Buffer buf) {
    return (Parse){
        .start = (char *)buf.ptr,
        .end = (char *)buf.ptr + buf.size,
        .cursor = (char *)buf.ptr,
    };
}

// Indicate that parsing the current expression failed
// it is possible to backtrack by restoring a previous 'parser_save' with 'parser_restore'
static void parse_fail(Parse *p) {
    p->error = 1;
}

// Look at the next char without advancing parser state
static char parse_peek(Parse *p) {
    if (p->error)
        return 0;
    if (p->cursor == p->end)
        return 0;
    return *p->cursor;
}

// advance the parser one character and return it
static char parse_char(Parse *p) {
    // already failed
    if (p->error)
        return 0;

    // at the end -> fail
    if (p->cursor == p->end) {
        parse_fail(p);
        return 0;
    }

    // ok
    char c = *p->cursor++;
    if (c == '\n')
        p->line++;
    return c;
}

// Save current parser state, to be restored at any time
// This is very cheap (just saving the struct on the stack)
// But parse_peek is (probably) still faster when possible.
static Parse parse_save(Parse *p) {
    return *p;
}

// Restore the parser to a previously saved state
static void parse_restore(Parse *p, Parse prev) {
    *p = prev;
}

// Are we done with parsing?
static bool parse_is_done(Parse *p) {
    return p->cursor == p->end || p->error;
}

// Show the error location where the parse failed.
static void parse_format_result(Parse *parse, Format *f) {
    if (!parse->error) {
        fmt_str(f, "Parsing ok.\n");
        return;
    }

    fmt_str(f, "Parsing failed at line ");
    fmt_u64(f, parse->line);
    fmt_str(f, " p=");

    fmt_base(f, 16);
    f->prefix_char = 'x';
    fmt_pad(f, 2);

    fmt_u64(f, *parse->cursor);
    fmt_str(f, ": \n");

    // Find start of the line
    char *line_start = (char *)parse->cursor;
    for (;;) {
        if (line_start == (char *)parse->start)
            break;
        if (line_start[-1] == '\n')
            break;
        line_start--;
    }

    // Show arrow pointing at the character
    u32 offset = parse->cursor - line_start;
    for (u32 i = 1; i < offset; ++i)
        fmt_str(f, "-");
    fmt_str(f, "v\n");

    // Print the line
    fmt_str_len(f, str_find(line_start, '\n'), line_start);
    fmt_chr(f, '\n');
}

// Combinators
static char parse_digit(Parse *p) {
    char d = parse_char(p);
    if (d >= '0' && d <= '9')
        return d - '0';
    parse_fail(p);
    return 0;
}

static void parse_match(Parse *p, char c) {
    if (parse_char(p) == c)
        return;
    parse_fail(p);
}

static void parse_symbol(Parse *p, char *sym) {
    while (*sym) {
        if (parse_char(p) != *sym++) {
            parse_fail(p);
            return;
        }
    }
}

static void parse_space(Parse *p) {
    char c = parse_char(p);
    if (c != ' ') {
        parse_fail(p);
        return;
    }
    while (parse_peek(p) == ' ')
        parse_char(p);
}

static void parse_whitespace(Parse *p) {
    for (;;) {
        char c = parse_peek(p);
        if (c != ' ' && c != '\n' && c != '\t' && c != '\r') {
            return;
        }
        parse_char(p);
    }
}

// ==== Token ====
enum Token_Type {
    Token_None,
    Token_Symbol,
    Token_Number,
    Token_String,
    Token_Char,
    Token_Comment,
    Token_Op,
    Token_Macro,
};
typedef enum Token_Type Token_Type;

struct Token {
    u32 type;
    char *str;
    Token *next;
};

static bool is_op(char c) {
    char ops[] = "!%&()*+,-./:;<=>?[\\]^_{|}~";
    return str_chr(ops, c);
}

static bool tok_is_op(Token *tok, char *op) {
    return tok->type == Token_Op && str_eq(tok->str, op);
}

static bool tok_is_symbol(Token *tok, char *sym) {
    return tok->type == Token_Symbol && str_eq(tok->str, sym);
}

static Token *parse_token(mem *m, char *str) {
    Token *first = 0;
    Token *last = 0;
    for (;;) {
        Token_Type type = 0;

        // consume whitespace
        while (is_whitespace(*str))
            str++;

        char *tok_start = str;
        char c0 = str[0];
        if (c0 == 0)
            break;

        char c1 = str[1];
        if (is_alpha(c0)) {
            // some identifier
            type = Token_Symbol;
            do {
                str++;
            } while (is_alpha_numeric(*str));
        } else if (is_digit(c0)) {
            // Number
            type = Token_Number;
            do {
                str++;
            } while (is_alpha_numeric(*str));
        } else if (c0 == 0x22 || c0 == 0x27) {
            // String or char
            type = Token_String;
            do {
                str++;
            } while (!is_eol(*str) && *str != c0);
            str++;
        } else if (c0 == '/' && c1 == '/') {
            // comment
            type = Token_Comment;
            str++;
            str++;
            while (!is_eol(*str))
                str++;

            // Maybe we could include comments
            continue;
        } else if (c0 == '/' && c1 == '*') {
            // block comment
            type = Token_Comment;
            str++;
            str++;
            for (;;) {
                if (*str == 0)
                    break;

                if (str[-1] == '*' && str[0] == '/') {
                    str++;
                    break;
                }
                str++;
            }
            continue;
        } else if (c0 == '#') {
            // Define, or import
            type = Token_Macro;
            while (!is_eol(*str))
                str++;
        } else if (is_op(c0)) {
            type = Token_Op;
            str++;
        } else {
            os_printf("Unexpected token: '%c'\n", c0);
            break;
        }
        char *tok_end = str;

        // Nothing parsed, we are at the end
        if (tok_start == tok_end)
            break;

        Token *tok = mem_struct(m, Token);
        tok->type = type;
        tok->str = str_dup_len(m, tok_start, tok_end - tok_start);

        // append token
        if (last) {
            last->next = tok;
            last = tok;
        } else {
            first = tok;
            last = tok;
        }
    }

    return first;
}
