#pragma once
#include "ledger/parse.h"
#include "tlib/inc.h"
#include "tlib/parse.h"

struct Date {
    u32 year;
    u32 month;
    u32 day;
};

struct Balance {
    Memory *mem;
    Balance_Item *items;
};

struct Balance_Item {
    char *name;
    Date date;
    u64 input;
    u64 output;
    Balance_Item *next;
};

struct Transaction {
    Date date;
    u64 amount;
    char *src;
    char *dst;
    Transaction *next;
};

struct Mapping {
    char *src;
    char *dst;
    Mapping *next;
};

// === Date ===
static Date parse_date(Parse *p) {
    Date date = {};
    date.year = parse_u64(p);
    parse_match(p, '-');
    date.month = parse_u64(p);
    parse_match(p, '-');
    date.day = parse_u64(p);
    return date;
}

static void fmt_date(Format *f, Date *d) {
    fmt_pad(f, 4);
    fmt_u64(f, d->year);
    fmt_str(f, "-");
    fmt_zero_pad(f, 2);
    fmt_u64(f, d->month);
    fmt_str(f, "-");
    fmt_zero_pad(f, 2);
    fmt_u64(f, d->day);
}

// === Money ===
static u64 parse_money(Parse *p) {
    u64 cents = parse_u64(p) * 100;
    if (parse_peek(p) == '.') {
        parse_char(p);
        cents += 10 * (u64)parse_digit(p) + (u64)parse_digit(p);
    }
    return cents;
}

static void fmt_money(Format *f, i64 cents) {
    fmt_i64(f, cents / 100);
    fmt_str(f, ".");
    fmt_zero_pad(f, 2);
    fmt_u64(f, i_abs(cents) % 100);
}

// === Transaction ===
static Transaction *parse_transaction(Parse *p, Memory *mem) {
    if (p->error)
        return 0;
    Transaction *ts = mem_struct(mem, Transaction);
    ts->date = parse_date(p);
    parse_space(p);
    ts->amount = parse_money(p);
    parse_space(p);
    ts->src = parse_ledger_word(p, mem);
    parse_space(p);
    ts->dst = parse_ledger_word(p, mem);
    parse_til_next_line(p, mem);
    return ts;
}

static void fmt_transaction(Format *f, Transaction *ts) {
    u32 pad = 0;
    fmt_date(f, &ts->date);
    fmt_str(f, " ");
    pad = fmt_pad_start(f);
    fmt_money(f, ts->amount);
    fmt_lpad(f, pad, 8);
    fmt_str(f, " ");
    pad = fmt_pad_start(f);
    fmt_str(f, ts->src);
    fmt_rpad(f, pad, 25);
    fmt_str(f, " ");
    pad = fmt_pad_start(f);
    fmt_str(f, ts->dst);
    fmt_rpad(f, pad, 25);
    fmt_str(f, "\n");
}

// == Mapping ==
static Mapping *parse_mapping(Parse *p, Memory *mem) {
    if (p->error) return 0;
    Mapping *map = mem_struct(mem, Mapping);

    map->dst = parse_ledger_word(p, mem);
    parse_space(p);
    parse_ledger_word(p, mem);
    parse_space(p);
    map->src = parse_til_next_line(p, mem);
    return map;
}

static void fmt_mapping(Format *f, Mapping *map) {
    u32 pad = 0;

    pad = fmt_pad_start(f);
    fmt_str(f, map->src);
    fmt_rpad(f, pad, 25);
    fmt_str(f, " ");
    fmt_str(f, map->dst);
    fmt_str(f, "\n");
}
