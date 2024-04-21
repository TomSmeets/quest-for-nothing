#pragma once
#include "app/ledger/data.h"
#include "tlib/parse.h"

static u64 parse_u64(Parse *p) {
    u64 n = parse_digit(p);
    if (p->error)
        return 0;

    for (;;) {
        char c = parse_peek(p);
        if (!(c >= '0' && c <= '9'))
            break;
        parse_char(p);
        n = n * 10 + (c - '0');
    }
    return n;
}

static bool is_ledger_word_char(char c) {
    if (is_alpha(c))
        return 1;
    if (c == '_') return 1;
    if (c == '.') return 1;
    if (c == '-') return 1;
    if (c == '?') return 1;
    if (c == '*') return 1;
    return 0;
}

static char *parse_ledger_word(Parse *p, Memory *m) {
    char *start = p->cursor;
    if (!is_ledger_word_char(parse_peek(p))) {
        parse_fail(p);
        return 0;
    }
    while (is_ledger_word_char(parse_peek(p)))
        parse_char(p);
    char *end = p->cursor;
    return str_dup_len(m, start, end - start);
}

static char *parse_til_next_line(Parse *p, Memory *m) {
    if (p->error)
        return 0;

    char *start = p->cursor;
    for (;;) {
        char c = parse_char(p);
        if (c == '\n')
            break;
        if (c == 0)
            break;
    }
    if (start == p->cursor)
        return 0;
    char *end = p->cursor - 1;
    return str_dup_len(m, (char *)start, end - start);
}

static void parse_csv_start(Parse *p) {
    parse_match(p, '"');
}

static void parse_csv_end(Parse *p) {
    parse_match(p, '"');
    if (p->error)
        return;
    Parse state = parse_save(p);
    parse_match(p, ';');
    if (!p->error)
        return;
    parse_restore(p, state);
    parse_match(p, '\r');
    parse_match(p, '\n');
    if (!p->error)
        return;
    parse_restore(p, state);
    parse_match(p, '\n');
}

static char *parse_csv_string(Parse *p, Memory *m) {
    if (p->error)
        return 0;
    parse_csv_start(p);

    char *start = p->cursor;
    while (parse_peek(p) != '\"' && !p->error) {
        parse_char(p);
    }
    char *end = p->cursor;

    parse_csv_end(p);
    if (p->error)
        return 0;

    if (start == end)
        return 0;
    return str_dup_len(m, start, end - start);
}

static bool parse_af_bij(Parse *p) {
    parse_csv_start(p);
    Parse save = parse_save(p);

    parse_symbol(p, "Af");
    parse_csv_end(p);
    if (!p->error)
        return 1;

    parse_restore(p, save);
    parse_symbol(p, "Bij");
    parse_csv_end(p);
    return 0;
}

