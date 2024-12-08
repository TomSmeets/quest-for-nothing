#pragma once
#include "fmt.h"
#include "mem.h"
#include "str.h"

// ./main <action> [options...]
typedef struct Cli_Action Cli_Action;
struct Cli_Action {
    char *name;
    char *args;
    char *description;
    Cli_Action *next;
};

typedef struct {
    Memory *mem;

    u32 argc;
    char **argv;

    Cli_Action *action_list;
    Cli_Action *action_list_last;
} Cli;

static bool cli_action(Cli *cli, char *name, char *args, char *description) {
    Cli_Action *act = mem_struct(cli->mem, Cli_Action);
    act->name = name;
    act->args = args;
    act->description = description;
    LIST_APPEND(cli->action_list, cli->action_list_last, act);

    // Check exact match
    return cli->argc >= 2 && str_eq(cli->argv[1], name);
}

static u32 cli_arg_help_len(Cli_Action *act) {
    u32 len_name = str_len(act->name);
    u32 len_arg = str_len(act->args);
    return len_name + len_arg;
}

static void cli_show_help(Cli *cli) {
    Fmt *fmt = OS_FMT;
    char *name = cli->argv[0];
    fmt_ss(fmt, "Usage: ", name, " <action> [args]...\n");
    fmt_s(fmt, "\n");
    fmt_s(fmt, "Actions:\n");

    u32 max_len = 0;
    for (Cli_Action *act = cli->action_list; act; act = act->next) {
        u32 len = cli_arg_help_len(act);
        if (len > max_len) max_len = len;
    }

    for (Cli_Action *act = cli->action_list; act; act = act->next) {
        u32 len = cli_arg_help_len(act);
        u32 pad = max_len - len;
        fmt_s(fmt, "  ");
        fmt_s(fmt, act->name);
        fmt_s(fmt, " ");
        fmt_s(fmt, act->args);
        fmt_s(fmt, " ");
        for (u32 i = 0; i < pad; ++i) {
            fmt_s(fmt, " ");
        }
        fmt_s(fmt, act->description);
        fmt_s(fmt, "\n");
    }
}

static Cli *cli_new(Memory *mem, OS *os) {
    Cli *cli = mem_struct(mem, Cli);
    cli->mem = mem;
    cli->argc = os->argc;
    cli->argv = os->argv;
    return cli;
}
