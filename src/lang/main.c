#include "lib/fmt.h"
#include "lib/os_main.h"
#include "lib/str.h"

typedef struct {
    Memory *mem;
    u32 index;
    u32 len;
    u8 *data;
} Parse;

typedef struct Expr Expr;

struct Expr {
    char op;
    i32 value;
    Expr *left;
    Expr *right;
};

static u8 parse_peek(Parse *parse) {
    if (parse->index >= parse->len) return 0;
    return parse->data[parse->index];
}

static void parse_next(Parse *parse) {
    if (parse->index < parse->len) parse->index++;
}

static void parse_space(Parse *parse) {
    for (;;) {
        u8 c = parse_peek(parse);
        if (c == 0) break;
        if (c != ' ') break;
        parse_next(parse);
    }
}

static Expr *parse_num(Parse *parse) {
    i32 value = 0;
    bool valid = 0;
    for (;;) {
        u8 c = parse_peek(parse);
        bool is_digit = c >= '0' && c <= '9';

        if (!is_digit) break;

        parse_next(parse);
        valid = 1;
        value *= 10;
        value += c - '0';
    }

    parse_space(parse);
    if (!valid) return 0;

    Expr *expr = mem_struct(parse->mem, Expr);
    expr->value = value;
    return expr;
}

static void fmt_expr(Fmt *fmt, Expr *expr) {
    if (!expr) {
        fmt_s(fmt, "#ERROR");
        return;
    }

    if (expr->op) {
        fmt_c(fmt, '(');
        fmt_expr(fmt, expr->left);
        fmt_c(fmt, ' ');
        fmt_c(fmt, expr->op);
        fmt_c(fmt, ' ');
        fmt_expr(fmt, expr->right);
        fmt_c(fmt, ')');
    } else {
        fmt_i(fmt, expr->value);
    }
}

static Expr *parse_expr(Parse *parse);

static Expr *parse_bracket(Parse *parse) {
    if (parse_peek(parse) != '(') return 0;
    parse_next(parse);
    parse_space(parse);
    Expr *expr = parse_expr(parse);
    if (parse_peek(parse) != ')') return 0;
    parse_next(parse);
    parse_space(parse);
    return expr;
}

static Expr *parse_lit(Parse *parse) {
    Expr *expr = parse_num(parse);
    if (expr) return expr;
    expr = parse_bracket(parse);
    return expr;
}

static Expr *parse_mul(Parse *parse) {
    Expr *expr = parse_lit(parse);
    for(;;) {
        if (!expr) break;
        u8 op_chr = parse_peek(parse);

        if (op_chr != '*' && op_chr != '/') break;
        parse_next(parse);
        parse_space(parse);

        Expr *right = parse_lit(parse);

        Expr *op = mem_struct(parse->mem, Expr);
        op->left = expr;
        op->right = right;
        op->op = op_chr;
        expr = op;
    }
    return expr;
}

static Expr *parse_add(Parse *parse) {
    Expr *expr = parse_mul(parse);
    for(;;) {
        if (!expr) break;

        u8 op_chr = parse_peek(parse);
        if (op_chr != '+' && op_chr != '-') break;
        parse_next(parse);
        parse_space(parse);

        Expr *right = parse_mul(parse);

        Expr *op = mem_struct(parse->mem, Expr);
        op->left = expr;
        op->right = right;
        op->op = op_chr;
        expr = op;
    }
    return expr;
}

static Expr *parse_expr(Parse *parse) {
    return parse_add(parse);
}

static i32 expr_eval(Expr *expr) {
    if (!expr) return 0;
    if (expr->op) {
        i64 left = expr_eval(expr->left);
        i64 right = expr_eval(expr->right);
        if (expr->op == '+') return left + right;
        if (expr->op == '-') return left - right;
        if (expr->op == '*') return left * right;
        if (expr->op == '/') return left / right;
    } else {
        return expr->value;
    }
    return 0;
}

static void os_main(void) {
    // 1. Read line
    String line = S("1 + 2 * 3 * 8 + 4 + 5");
    Parse parse = {
        .mem = mem_new(),
        .data = line.data,
        .len = line.len,
    };

    // 2. parse into ast
    parse_space(&parse);
    Expr *expr = parse_expr(&parse);
    fmt_s(G->fmt, "Ast: ");
    fmt_expr(G->fmt, expr);
    fmt_s(G->fmt, "\n");

    // 3. evalute
    fmt_s(G->fmt, "Value: ");
    fmt_i(G->fmt, expr_eval(expr));
    fmt_s(G->fmt, "\n");

    // 4. print result
    os_exit(0);
}
