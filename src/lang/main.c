#include "lib/fmt.h"
#include "lib/mem.h"
#include "lib/os_main.h"
#include "lib/str.h"

typedef struct {
    Memory *mem;
    u32 index;
    u32 len;
    u8 *data;
} Parse;

typedef enum {
    Token_Eof,
    Token_Add,
    Token_Sub,
    Token_Mul,
    Token_Div,
    Token_Num,
    Token_BrOpen,
    Token_BrClose,
    Token_Semi, // Blocks
} Token_Type;

typedef struct Expr Expr;

typedef struct Token Token;
struct Token {
    String text;
    Token *next;
};

struct Expr {
    // Token
    Token_Type token_type;
    String token;
    Expr *next;

    // Lit
    i32 value;

    // Binop
    Expr *left;
    Expr *right;

    // Error message
    char *error;
};

static Expr *parse_tokens(Memory *mem, String str) {
    Expr *tokens_start = 0;
    Expr *tokens_end = 0;

    u32 index = 0;
    for (;;) {
        u32 start = index;

        // End of file
        if (index == str.len) break;
        u8 first = str.data[index++];

        // Consume whitespace
        if (first == ' ' || first == '\n' || first == '\r') continue;

        // Next is some token
        Expr *token = mem_struct(mem, Expr);
        if (0) {
        } else if (first == '+') {
            token->token_type = Token_Add;
        } else if (first == '-') {
            token->token_type = Token_Sub;
        } else if (first == '*') {
            token->token_type = Token_Mul;
        } else if (first == '/') {
            token->token_type = Token_Div;
        } else if (first == '(') {
            token->token_type = Token_BrOpen;
        } else if (first == ')') {
            token->token_type = Token_BrClose;
        } else if (first == ';') {
            token->token_type = Token_Semi;
        } else if (first >= '0' && first <= '9') {
            token->token_type = Token_Num;
            for (;;) {
                if (index == str.len) break;
                u8 chr = str.data[index];
                bool is_number = chr >= '0' && chr <= '9';
                if (!is_number) break;
                index++;
            }
        } else {
            token->error = "Invalid Token";
        }
        token->token = str_slice(str, start, index - start);
        LIST_APPEND(tokens_start, tokens_end, token);
    }

    {
        Expr *eof = mem_struct(mem, Expr);
        eof->token = S("EOF");
        eof->token_type = Token_Eof;
        LIST_APPEND(tokens_start, tokens_end, eof);
    }

    return tokens_start;
}

static void fmt_tokens(Fmt *fmt, Expr *expr) {
    while (expr) {
        fmt_str(fmt, expr->token);
        fmt_s(fmt, " ");
        expr = expr->next;
    }
}

static void fmt_expr(Fmt *fmt, Expr *expr) {
    if (!expr) {
        fmt_s(fmt, "#ERROR");
        return;
    }

    switch(expr->token_type) {
        case Token_Eof:
            fmt_s(fmt, "EOF");
            break;
        case Token_Add:
        case Token_Sub:
        case Token_Mul:
        case Token_Div:
            fmt_c(fmt, '(');
            fmt_expr(fmt, expr->left);
            fmt_c(fmt, ' ');
            fmt_str(fmt, expr->token);
            fmt_c(fmt, ' ');
            fmt_expr(fmt, expr->right);
            fmt_c(fmt, ')');
            break;
        case Token_Num:
            fmt_i(fmt, expr->value);
            break;
        case Token_Semi:
            fmt_expr(fmt, expr->left);
            fmt_str(fmt, expr->token);
            fmt_expr(fmt, expr->right);
            break;
        case Token_BrOpen:
        case Token_BrClose:
            break;
    }

    if(expr->error) {
        fmt_s(fmt, "ERR(");
        fmt_s(fmt, expr->error);
        fmt_s(fmt, ")");
    }
}

typedef struct {
    Memory *mem;
    Expr *token;
} ExprParse;

static Token_Type parse_peek(ExprParse *parse) {
    return parse->token->token_type;
}

static Expr *parse_next(ExprParse *parse) {
    Expr *exp = parse->token;
    parse->token = parse->token->next;
    return exp;
}

static Expr *parse_expr(ExprParse *parse);

static Expr *parse_num(ExprParse *parse) {
    if (parse_peek(parse) != Token_Num) return 0;
    Expr *token = parse_next(parse);
    i32 value = 0;
    for (u32 i = 0; i < token->token.len; ++i) {
        value *= 10;
        value += token->token.data[i] - '0';
    }
    token->value = value;
    return token;
}

static Expr *parse_bracket(ExprParse *parse) {
    if (parse_peek(parse) != Token_BrOpen) return 0;
    parse_next(parse);

    Expr *expr = parse_expr(parse);

    if (parse_peek(parse) != Token_BrClose) {
        parse_next(parse)->error = "Expecting ')'";
        return 0;
    }
    parse_next(parse);
    return expr;
}

static Expr *parse_lit(ExprParse *parse) {
    Expr *expr = 0;
    if ((expr = parse_num(parse))) return expr;
    if ((expr = parse_bracket(parse))) return expr;
    return 0;
}

static Expr *parse_mul(ExprParse *parse) {
    Expr *expr = parse_lit(parse);
    for (;;) {
        if (!expr) break;
        Token_Type op_chr = parse_peek(parse);
        if (op_chr != Token_Mul && op_chr != Token_Div) break;
        Expr *op = parse_next(parse);
        Expr *right = parse_lit(parse);
        op->left = expr;
        op->right = right;
        expr = op;
    }
    return expr;
}

static Expr *parse_add(ExprParse *parse) {
    Expr *expr = parse_mul(parse);
    for (;;) {
        if (!expr) break;
        Token_Type op_chr = parse_peek(parse);
        if (op_chr != Token_Add && op_chr != Token_Sub) break;
        Expr *op = parse_next(parse);
        Expr *right = parse_mul(parse);
        op->left = expr;
        op->right = right;
        expr = op;
    }
    return expr;
}

static Expr *parse_expr(ExprParse *parse) {
    return parse_add(parse);
}

static Expr *parse_statement(ExprParse *parse) {
    Expr *expr = parse_expr(parse);
    Expr *semi = parse_next(parse);

    if(semi->token_type != Token_Semi) {
        semi->error = "Expecting ';'";
    }

    semi->left = expr;
    semi->right = 0;
    return expr;
}

static Expr *parse_lang(ExprParse *parse) {
    Expr *block_first = 0;
    Expr *block_last = 0;
    while (parse_peek(parse) != Token_Eof) {
        Expr *stm = parse_statement(parse);
        LIST_APPEND2(block_first, block_last, stm, right);
    }
    return block_first;
}

static i32 expr_eval(Expr *expr) {
    if (!expr) return 0;
    switch(expr->token_type) {
    case Token_Eof:
    case Token_Add:
        return expr_eval(expr->left) + expr_eval(expr->right);
    case Token_Sub:
        return expr_eval(expr->left) - expr_eval(expr->right);
    case Token_Mul:
        return expr_eval(expr->left) * expr_eval(expr->right);
    case Token_Div:
        return expr_eval(expr->left) / expr_eval(expr->right);
    case Token_Num:
        return expr->value;
    case Token_BrOpen:
    case Token_BrClose:
        break;
    case Token_Semi:
        return expr_eval(expr->left) + expr_eval(expr->right);
    }
    return 0;
}

static void os_main(void) {
    // 1. Read line
    String line = S("1 + 2*2 + 3 + 4 + 5; 3 + 2");
    Memory *mem = mem_new();

    Expr *tokens = parse_tokens(mem_new(), line);
    fmt_s(G->fmt, "Tokens: ");
    fmt_tokens(G->fmt, tokens);
    fmt_s(G->fmt, "\n");

    ExprParse parse = {
        .mem = mem,
        .token = tokens,
    };

    // 2. parse into ast
    Expr *expr = parse_lang(&parse);
    fmt_s(G->fmt, "Ast: ");
    fmt_expr(G->fmt, expr);
    fmt_s(G->fmt, "\n");

    // 3. evalute
    fmt_s(G->fmt, "Value: ");
    fmt_i(G->fmt, expr_eval(expr));
    fmt_s(G->fmt, "\n");
    fmt_s(G->fmt, "ASM:\n");

    // 4. print result
    os_exit(0);
}
