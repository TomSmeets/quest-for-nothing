#pragma once
#include "fmt.h"
#include "mem.h"
#include "str.h"

// ./main <action> [options...]
typedef struct {
    char *name;
    char *description;
} Cli_Action;

typedef struct {
    u32 argc;
    char **argv;

    u32 cursor;

    u32 action_count;
    Cli_Action actions[16];
} Cli;

static bool cli_action(Cli *cli, char *name, char *description) {
    assert(cli->action_count < array_count(cli->actions), "Too many cli actions");

    Cli_Action opt = {.name = name, .description = description};
    cli->actions[cli->action_count++] = opt;

    return cli->argc >= 2 && str_eq(cli->argv[1], name);
}

static void cli_show_help(Cli *cli) {
    Fmt *fmt = OS_FMT;
    fmt_ss(fmt, "Usage: ", cli->argv[0], " <action> [options...]\n");
    fmt_s(fmt, cli->argv[0]);
}

struct Arg_Parser {
    Memory *mem;

    u32 argc;
    char **argv;

    Arg_Parser_Arg *args;
    Arg_Parser_Arg *args_last;
    u32 found_count;
};

// Add argument, and return index, or 0
static u32 arg_add(Arg_Parser *p, char *s_short, char *s_long, char *extra, char *description) {
    Arg_Parser_Arg *a = mem_struct(p->mem, Arg_Parser_Arg);
    a->s_short = s_short;
    a->s_long = s_long;
    a->description = description;
    a->extra = extra;
    list_append(p->args, p->args_last, a);

    for (u32 i = 1; i < p->argc; ++i) {
        if (!p->argv[i]) continue;
        bool is_short = str_eq(p->argv[i], s_short);
        bool is_long = str_eq(p->argv[i], s_long);
        if (is_short || is_long) {
            p->found_count++;
            p->argv[i] = 0;
            return i;
        }
    }
    return 0;
}

static bool arg_flag(Arg_Parser *p, char *s_short, char *s_long, char *description) {
    u32 ix = arg_add(p, s_short, s_long, 0, description);
    return ix > 0;
}

static char *arg_str(Arg_Parser *p, char *s_short, char *s_long, char *extra, char *description) {
    u32 ix = arg_add(p, s_short, s_long, extra, description);
    if (ix > 0 && ix + 1 < p->argc) {
        char *extra = p->argv[ix + 1];
        if (extra) {
            p->argv[ix + 1] = 0;
            return extra;
        }
    }
    return 0;
}

static void arg_print_help(Arg_Parser *p) {
    Format *f = fmt_new(p->mem);
    fmt_str(f, p->argv[0]);
    fmt_str(f, " [options]\n\n");
    fmt_str(f, "Usage:\n");
    for (Arg_Parser_Arg *a = p->args; a; a = a->next) {
        u32 pad_start = fmt_pad_start(f);
        fmt_str(f, "  ");
        fmt_str(f, a->s_short);
        fmt_str(f, ", ");
        fmt_str(f, a->s_long);
        if (a->extra) {
            fmt_str(f, " ");
            fmt_str(f, a->extra);
        }
        fmt_rpad(f, pad_start, 25);
        fmt_str(f, a->description);
        fmt_str(f, "\n");
    }
    os_print(fmt_end(f));
}

static bool arg_is_done(Arg_Parser *arg) {
    for (u32 i = 1; i < arg->argc; ++i) {
        if (arg->argv[i]) return 0;
    }
    return 1;
}
