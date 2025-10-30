// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// cli.h: Command line argument reader
#pragma once
#include "lib/fmt.h"
#include "lib/test.h"
#include "lib/types.h"

TYPEDEF_STRUCT(Cli_Option);
struct Cli_Option {
    char *name;
    char *info;
    Cli_Option *next;
};

// ./main [command] <input0> <input1> ... [flag1] [flag2] ...
TYPEDEF_STRUCT(Cli);
struct Cli {
    Memory *mem;
    u32 ix;
    u32 argc;
    char **argv;
    bool has_match;

    Cli_Option *options;
    Cli_Option *options_last;
};

static Cli *cli_from(Memory *mem, u32 argc, char **argv) {
    Cli *cli = mem_struct(mem, Cli);
    cli->mem = mem;
    cli->argc = argc;
    cli->argv = argv;
    cli->ix = 1;
    return cli;
}

static Cli *cli_new(void) {
    return cli_from(G->tmp, G->argc, G->argv);
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
    cli->options = 0;
    cli->options_last = 0;
}

static void cli_doc_add(Cli *cli, char *name, char *description) {
    Cli_Option *opt = mem_struct(cli->mem, Cli_Option);
    opt->name = name;
    opt->info = description;
    LIST_APPEND(cli->options, cli->options_last, opt);
}

// Check for a given 'flag'
// returns true on match, and advances to the next argument
// returns false when no match, stays at the same argument
static bool cli_match(Cli *cli, char *name, char *description) {
    if (cli->has_match) return false;

    cli_doc_add(cli, name, description);

    char *arg = cli_read(cli);
    if (!arg) return false;

    if (strz_eq(arg, name)) {
        cli_doc_clear(cli);
        cli_next(cli);
        cli->has_match = true;
        return true;
    }

    return false;
}

static char *cli_value(Cli *cli, char *name, char *description) {
    char *match = cli_next(cli);
    cli_doc_add(cli, name, description);
    if (match) cli_doc_clear(cli);
    return match;
}

// Show command usage and exit application
static void cli_show_help_and_exit(Cli *cli) {
    Fmt *fmt = G->fmt;
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
    for (Cli_Option *opt = cli->options; opt; opt = opt->next) {
        fmt_s(fmt, "    ");
        u32 cur = fmt_cursor(fmt);
        fmt_s(fmt, opt->name);
        fmt_pad(fmt, cur, ' ', 16, false);
        fmt_s(fmt, opt->info);
        fmt_s(fmt, "\n");
    }

    // Exit
    os_exit(1);
}

static void cli_test(Test *test) {
    Cli *cli = cli_from(test->mem, 5, (char *[]){"cli_test", "hello", "world", "--x", "--y"});
    TEST(cli_match(cli, "test", "Test") == 0);
    TEST(cli_match(cli, "world", "World") == 0);
    TEST(cli_match(cli, "hello", "Hello") == 1);
    TEST(cli_match(cli, "hi", "Hi") == 0);
    TEST(cli->has_match == 1);
}
