#pragma once
#include "app/ledger/balance.h"
#include "app/ledger/data.h"
#include "app/ledger/parse.h"

static Date parse_csv_date(Parse *p) {
    Date ret = {};
    parse_csv_start(p);
    ret.year = 10 * ret.year + parse_digit(p);
    ret.year = 10 * ret.year + parse_digit(p);
    ret.year = 10 * ret.year + parse_digit(p);
    ret.year = 10 * ret.year + parse_digit(p);
    ret.month = 10 * ret.month + parse_digit(p);
    ret.month = 10 * ret.month + parse_digit(p);
    ret.day = 10 * ret.day + parse_digit(p);
    ret.day = 10 * ret.day + parse_digit(p);
    parse_csv_end(p);
    return ret;
}

static u64 parse_csv_money(Parse *p) {
    parse_csv_start(p);
    u64 cents = 0;
    while (is_digit(parse_peek(p))) {
        cents = cents * 10 + parse_digit(p);
    }
    cents *= 100;
    parse_match(p, ',');
    if (is_digit(parse_peek(p)))
        cents += parse_digit(p) * 10;
    if (is_digit(parse_peek(p)))
        cents += parse_digit(p);
    parse_csv_end(p);
    return cents;
}

static Transaction *parse_csv_row(Parse *p, Memory *mem, Mapping *map) {
    // extract csv strings
    Date date = parse_csv_date(p);
    char *name = parse_csv_string(p, mem);
    char *iban_src = parse_csv_string(p, mem);
    char *iban_dst = parse_csv_string(p, mem);
    char *code = parse_csv_string(p, mem);
    bool is_af = parse_af_bij(p);
    u64 bedrag = parse_csv_money(p);
    char *soort = parse_csv_string(p, mem);
    char *desciption = parse_csv_string(p, mem);
    i64 saldo = parse_csv_money(p);
    parse_csv_string(p, mem);

    if (p->error) return 0;

    char *tag = "?";
    for(Mapping *m = map; m; m = m->next) {
        if(str_contains(name, m->src)) {
            tag = m->dst;
            break;
        }
    }

    char *src, *dst;
    if (is_af) {
        src = "betaal";
        dst = tag;
    } else {
        src = tag;
        dst = "betaal";
    }

    Transaction *ts = mem_struct(mem, Transaction);
    ts->date = date;
    ts->amount = bedrag;
    ts->src = src;
    ts->dst = dst;
    return ts;
}
