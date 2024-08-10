#include "ledger/balance.h"
#include "ledger/data.h"
#include "ledger/parse.h"
#include "ledger/csv.h"
#include "tlib/arg.h"
#include "tlib/fmt.h"
#include "tlib/os_generic.h"
#include "tlib/parse.h"

static void parse_line(Parse *p) {
    for (;;) {
        char c = parse_char(p);
        if (c == 0 || c == '\n')
            return;
    }
}

static Parse parse_file(Memory *mem, char *file) {
    return parse_new(os_read_file(mem, file));
}

static Mapping *handle_mapping(Memory *mem, char *file) {
    Parse parse = parse_file(mem, file);
    Format *f = fmt_new(mem);

    Mapping *first = 0;
    Mapping *last  = 0;
    while (!parse_is_done(&parse) && !parse.error) {
        parse_whitespace(&parse);
        Mapping *map = parse_mapping(&parse, mem);
        if (parse.error) break;
        fmt_mapping(f, map);
        list_append(first, last, map);
    }

    if (parse.error)
        parse_format_result(&parse, f);

    os_print(fmt_end(f));
    return first;
}

static void handle_csv(Memory *mem, char *file, Mapping *mapping) {
    Parse parse = parse_file(mem, file);
    // skip header
    parse_line(&parse);

    Transaction *list = 0;
    while (!parse_is_done(&parse) && !parse.error) {
        Transaction *t = parse_csv_row(&parse, mem, mapping);

        // Prepend to list
        t->next = list;
        list = t;
    }

    Format *f = fmt_new(mem);
    if (parse.error) {
        parse_format_result(&parse, f);
    } else {
        for(Transaction *t = list; t; t = t->next) {
            fmt_transaction(f, t);
        }
    }
    os_print(fmt_end(f));
}

static void handle_file(Memory *mem, Balance *balance, char *file) {
    Parse parse = parse_file(mem, file);
    Format *f = fmt_new(mem);

    while (!parse_is_done(&parse) && !parse.error) {
        parse_whitespace(&parse);
        Transaction *ts = parse_transaction(&parse, mem);
        if (parse.error) break;
        fmt_transaction(f, ts);
        balance_add(balance, ts);
    }

    if (parse.error)
        parse_format_result(&parse, f);

    os_print(fmt_end(f));
}


void *main_init(int argc, char **argv) {
    // Main memory
    Memory *mem = mem_new();
    Balance balance = {.mem = mem};
    Arg_Parser arg = {.mem = mem, .argc = argc, .argv = argv};

    char *input_file    = arg_str(&arg,  "-f", "--file", "FILE", "Input file");
    char *input_csv     = arg_str(&arg,  "-c", "--csv", "FILE", "Input CSV file");
    char *input_mapping = arg_str(&arg,  "-m", "--mapping", "FILE", "Name mapping file");
    bool show_help      = arg_flag(&arg, "-h", "--help", "Show this help message");

    u32 file_count = (u32)(input_file == 0) + (u32)(input_csv == 0);
    if (show_help || (file_count != 1) || !arg_is_done(&arg)) {
        arg_print_help(&arg);
        os_exit(1);
    }

    Mapping *mapping = 0;
    if(input_mapping) mapping = handle_mapping(mem, input_mapping);
    if (input_file) handle_file(mem, &balance, input_file);
    if (input_csv) handle_csv(mem, input_csv, mapping);

    if (0) {
        Date date = {0};
        Format *f = fmt_new(mem);
        // balance_sort(&balance);
        for (Balance_Item *i = balance.items; i; i = i->next) {
            os_print("HI\n");
            if (i->date.month != date.month || i->date.year != date.year) {
                date = i->date;
                fmt_str(f, "\n");
                fmt_f(f, "==== Balance %4d / %02d ====\n", date.year, date.month);
            }
            fmt_pad(f, 32);
            fmt_str(f, i->name);
            fmt_str(f, "|");
            fmt_pad(f, 13);
            fmt_money(f, (i64)i->input - (i64)i->output);
            fmt_str(f, "\n");
        }
        os_print(fmt_end(f));
    }

    mem_free(mem);
    return 0;
}

void main_update(void *handle) {
    os_exit(0);
}
