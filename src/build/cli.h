// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// cli.h: Command line argument reader
#pragma once
#include "lib/fmt.h"
#include "lib/types.h"

TYPEDEF_STRUCT(Cli);

struct Cli {
    u32 ix;
    u32 argc;
    char **argv;
    bool has_match;

    u32 option_count;
    char *option_list[64][2];
};

static Cli *cli_new(void) {
    Memory *mem = G->mem;
    Cli *cli = mem_struct(mem, Cli);
    cli->argc = G->argc;
    cli->argv = G->argv;
    cli->ix = 1;
    return cli;
}

// Read value of current argument
static char *cli_read(Cli *cli) {
    if (cli->ix == cli->argc) return 0;
    return cli->argv[cli->ix];
}

// Read the value of current argument and advance to the next argument
static char *cli_next(Cli *cli) {
    if (cli->ix == cli->argc) return 0;
    return cli->argv[cli->ix++];
}

static void cli_doc_clear(Cli *cli) {
    cli->option_count = 0;
}

static void cli_doc_add(Cli *cli, char *name, char *description) {
    if (cli->option_count >= array_count(cli->option_list)) return;
    cli->option_list[cli->option_count][0] = name;
    cli->option_list[cli->option_count][1] = description;
    cli->option_count++;
}

// Check for a given 'flag'
// returns true on match, and advances to the next argument
// returns false when no match, stays at the same argument
static bool cli_flag(Cli *cli, char *name, char *description) {
    cli_doc_add(cli, name, description);

    char *arg = cli_read(cli);
    if (!arg) return false;

    if(cli->has_match) return false;

    if (strz_eq(arg, name)) {
        cli_doc_clear(cli);
        cli_next(cli);
        cli->has_match = true;
        return true;
    }

    return false;
}

static char *cli_value(Cli *cli, char *name, char *description) {
    cli_doc_clear(cli);
    cli_doc_add(cli, name, description);
    return cli_next(cli);
}

static void cli_show_usage(Cli *cli, Fmt *fmt) {
    u32 pos = 0;
    u32 len = 0;
    for (u32 i = 0; i < cli->argc + 1; ++i) {
        if (i == cli->ix) {
            pos = fmt_cursor(fmt);

            if (i < cli->argc) {
                len = str_len(cli->argv[i]);
            } else {
                len = 1;
            }
        }

        if (i < cli->argc) {
            fmt_s(fmt, cli->argv[i]);
            fmt_s(fmt, " ");
        }
    }
    fmt_s(fmt, "\n");
    for (u32 i = 0; i < pos; ++i) {
        fmt_s(fmt, " ");
    }
    for (u32 i = 0; i < len; ++i) {
        fmt_s(fmt, "^");
    }
    fmt_s(fmt, "\n");

    fmt_s(fmt, "Expecting one of:\n");
    for (u32 i = 0; i < cli->option_count; ++i) {
        fmt_s(fmt, "    ");
        u32 cur = fmt_cursor(fmt);
        fmt_s(fmt, cli->option_list[i][0]);
        fmt_pad(fmt, cur, ' ', 16, false);
        fmt_s(fmt, cli->option_list[i][1]);
        fmt_s(fmt, "\n");
    }
}
