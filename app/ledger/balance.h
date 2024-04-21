#pragma once
#include "app/ledger/data.h"
#include "tlib/fmt.h"
#include "tlib/str.h"

static bool balance_equal(Balance_Item *item, char *name, Date date) {
    if (!str_eq(item->name, name))
        return 0;

    if (item->date.year != date.year)
        return 0;

    if (item->date.month != date.month)
        return 0;

    return 1;
}

// Find a blance item for a given date
static Balance_Item *balance_get(Balance *b, char *name, Date date) {
    for (Balance_Item *item = b->items; item; item = item->next) {
        if (balance_equal(item, name, date))
            return item;
    }

    Balance_Item *item = mem_struct(b->mem, Balance_Item);
    item->name = str_dup(b->mem, name);
    item->date = date;
    item->next = b->items;
    b->items = item;
    return item;
}

static void balance_add(Balance *b, Transaction *ts) {
    balance_get(b, ts->src, ts->date)->input -= ts->amount;
    balance_get(b, ts->dst, ts->date)->input += ts->amount;
}
