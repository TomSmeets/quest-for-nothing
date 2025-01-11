// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// cli.h - Command line interface helper
#pragma once
#include "fmt.h"
#include "mem.h"
#include "str.h"

typedef struct Cli Cli;

static Cli *cli_new(Memory *mem, OS *os);

// Start this round of mathcing actions
static bool cli_begin(Cli *cli);

// Match an action
static bool cli_check(Cli *cli, char *name, char *args, char *description);

// Consume single argument to some action
static char *cli_next(Cli *cli);

// =================== Implementation =====================
typedef struct Cli_Action Cli_Action;
struct Cli_Action {
    char *name;
    char *args;
    char *description;
    Cli_Action *next;
};

struct Cli {
    Memory *mem;

    u32 argc;
    char **argv;
    u32 index;
    bool match;

    Cli_Action *action_list;
    Cli_Action *action_list_last;
};

static Cli *cli_new(Memory *mem, OS *os) {
    Cli *cli = mem_struct(mem, Cli);
    cli->mem = mem;
    cli->argc = os->argc;
    cli->argv = os->argv;
    cli->index = 0;
    cli->match = false;
    return cli;
}

// Get current argument
static char *cli_peek(Cli *cli) {
    if (cli->index == cli->argc) return 0;
    return cli->argv[cli->index];
}

// Advance to next argument
static char *cli_next(Cli *cli) {
    // Already at the end
    if (cli->index == cli->argc) return 0;
    cli->index++;
    return cli_peek(cli);
    return cli->argv[++cli->index];
}

// Check if current argument matces 'name'
static bool cli_check(Cli *cli, char *name) {
    char *arg = cli_peek(cli);
    if(!arg) return false;
    return str_eq(arg, name);
}

// Check if current argument matces, and add to help text
static bool cli_action(Cli *cli, char *name, char *args, char *description) {
    // Check arguemnt
    bool check = cli_check(cli, name);

    if(cli->index == 1) {
        // Append
        Cli_Action *act = mem_struct(cli->mem, Cli_Action);
        act->name = name;
        act->args = args;
        act->description = description;
        LIST_APPEND(cli->action_list, cli->action_list_last, act);
    }
    return check;
}

static u32 cli_arg_help_len(Cli_Action *act) {
    u32 len_name = str_len(act->name);
    u32 len_arg = str_len(act->args);
    return len_name + len_arg;
}

static void cli_begin(Cli *cli) {
    cli->match = false;
    cli->index++;
    if(cli->index >= cli->argc) return false;
}

static void cli_end(Cli *cli) {
    if(cli->match)
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
