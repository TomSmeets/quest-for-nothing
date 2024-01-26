// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// parse.h - A text parser
#pragma once
#include "inc.h"
#include "mem.h"
#include "str.h"
#include "fmt.h"

// ==== Simple Checks ===
static bool is_whitespace(u8 c) {
    if(c == ' ') return 1;
    if(c == '\n') return 1;
    if(c == '\t') return 1;
    if(c == '\r') return 1;
    return 0;
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

static bool is_eol(u8 c) {
    return c == 0 || c == '\n';
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
    Token *last  = 0;
    for(;;) {
        Token_Type type = 0;

        // consume whitespace
        while(is_whitespace(*str)) str++;

        char *tok_start = str;
        char c0 = str[0];
        if(c0 == 0) break;

        char c1 = str[1];
        if(is_alpha(c0)) {
            // some identifier
            type = Token_Symbol;
            do { str++; } while(is_alpha_numeric(*str));
        } else if(is_numeric(c0)) {
            // Number
            type = Token_Number;
            do { str++; } while(is_alpha_numeric(*str));
        } else if(c0 == 0x22 || c0 == 0x27) {
            // String or char
            type = Token_String;
            do { str++; } while(!is_eol(*str) && *str != c0);
            str++;
        } else if(c0 == '/' && c1 == '/') {
            // comment
            type = Token_Comment;
            str++;
            str++;
            while(!is_eol(*str)) str++;

            // Maybe we could include comments
            continue;
        } else if(c0 == '/' && c1 == '*') {
            // block comment
            type = Token_Comment;
            str++;
            str++;
            for(;;) {
                if(*str == 0) break;

                if(str[-1] == '*' && str[0] == '/') {
                    str++;
                    break;
                }
                str++;
            }
            continue;
        } else if(c0 == '#') {
            // Define, or import
            type = Token_Macro;
            while(!is_eol(*str)) str++;
        } else if(is_op(c0)) {
            type = Token_Op;
            str++;
        } else {
            os_printf("Unexpected token: '%c'\n", c0);
            break;
        }
        char *tok_end = str;

        // Nothing parsed, we are at the end
        if(tok_start == tok_end) break;

        Token *tok = mem_struct(m, Token);
        tok->type = type;
        tok->str  = str_dup_len(m, tok_start, tok_end - tok_start);

        // append token
        if(last) {
            last->next = tok;
            last = tok;
        } else {
            first = tok;
            last  = tok;
        }
    }

    return first;
}
