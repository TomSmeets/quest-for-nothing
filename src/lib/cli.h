// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// cli.h: Command line argument reader
#pragma once
#include "lib/fmt.h"
#include "lib/mem.h"
#include "lib/test.h"

TYPEDEF_STRUCT(Cli_Command);
TYPEDEF_STRUCT(Cli_Value);
TYPEDEF_STRUCT(Cli_Flag);
TYPEDEF_STRUCT(Cli_Arg);

struct Cli_Flag {
    char *name;
    char *info;
    bool match;
    Cli_Flag *next;
};

struct Cli_Value {
    char *name;
    char *info;
    char *match;
    Cli_Value *next;
};

struct Cli_Command {
    char *name;
    char *info;
    bool match;
    Cli_Flag *flag_first, *flag_last;
    Cli_Value *value_first, *value_last;
    Cli_Command *next;
};

struct Cli_Arg {
    char *value;
    bool used;
    bool flag;
    Cli_Arg *next;
};

typedef struct {
    Memory *mem;

    // argv
    char *arg_program;
    char *arg_command;
    Cli_Arg *arg_rest;

    // Help info
    Cli_Command *command_first;
    Cli_Command *command_last;
    bool has_match;
    bool show_help;
} Cli;

static bool cli_command(Cli *cli, char *name, char *info) {
    Cli_Command *cmd = mem_struct(cli->mem, Cli_Command);
    cmd->name = name;
    cmd->info = info;
    cmd->match = strz_eq(name, cli->arg_command);
    LIST_APPEND(cli->command_first, cli->command_last, cmd);

    if (cli->has_match) return false;
    if (cmd->match) cli->has_match = 1;

    return cmd->match;
}

static bool cli_flag(Cli *cli, char *name, char *info) {
    Cli_Command *cmd = cli->command_last;
    assert0(cmd);

    Cli_Flag *flag = mem_struct(cli->mem, Cli_Flag);
    flag->name = name;
    flag->info = info;
    LIST_APPEND(cmd->flag_first, cmd->flag_last, flag);

    if (cmd->match) {
        for (Cli_Arg *arg = cli->arg_rest; arg; arg = arg->next) {
            GUARD(!arg->used);
            GUARD(arg->flag);
            GUARD(strz_eq(arg->value, name));
            arg->used = true;
            flag->match = true;
            break;
        }
    }

    return flag->match;
}

static char *cli_value(Cli *cli, char *name, char *info) {
    Cli_Command *cmd = cli->command_last;
    assert0(cmd);

    Cli_Value *value = mem_struct(cli->mem, Cli_Value);
    value->name = name;
    value->info = info;
    LIST_APPEND(cmd->value_first, cmd->value_last, value);

    if (cmd->match) {
        for (Cli_Arg *arg = cli->arg_rest; arg; arg = arg->next) {
            GUARD(!arg->used);
            GUARD(!arg->flag);
            arg->used = true;
            value->match = arg->value;
            break;
        }
    }

    return value->match;
}

static Cli_Arg *arg_parse(Memory *mem, u32 argc, char **argv) {
    Cli_Arg *first = 0;
    Cli_Arg *last = 0;
    for (u32 i = 0; i < argc; ++i) {
        Cli_Arg *arg = mem_struct(mem, Cli_Arg);
        arg->value = argv[i];
        arg->flag = arg->value[0] == '-';
        LIST_APPEND(first, last, arg);
    }
    return first;
}

static Cli *cli_from(Memory *mem, u32 argc, char **argv) {
    Cli *cli = mem_struct(mem, Cli);
    cli->mem = mem;
    cli->arg_program = "";
    cli->arg_command = "";

    for (u32 i = 0; i < argc; ++i) {
        bool show_help = 0;
        if (strz_eq(argv[i], "--help")) show_help = 1;
        if (strz_eq(argv[i], "-?")) show_help = 1;
        if (!show_help) continue;

        cli->has_match = 1;
        cli->show_help = 1;

        break;
    }

    if (argc > 0) cli->arg_program = argv[0];
    if (argc > 1) cli->arg_command = argv[1];
    if (argc > 2) cli->arg_rest = arg_parse(mem, argc - 2, argv + 2);
    return cli;
}

static Cli *cli_new(void) {
    return cli_from(G->tmp, G->argc, G->argv);
}

// Check for the 'help' subcommand and handle no matches
// Call at the end for correct handling
static void cli_showhelp(Cli *cli, bool full) {
    u32 pad1 = 20;
    u32 pad2 = pad1 + 40;
    u32 pad3 = pad2 + 20;
    Fmt *f = G->fmt;

    // Check if there is a match
    bool has_command = !full;

    // Show current command
    if (!cli->has_match) {
        fmt_s(f, "Unknown Command: ");
        fmt_s(f, cli->arg_program);
        fmt_s(f, " ");
        fmt_s(f, cli->arg_command);
        for (Cli_Arg *arg = cli->arg_rest; arg; arg = arg->next) {
            fmt_s(f, " ");
            fmt_s(f, arg->value);
        }
        fmt_s(f, "\n");
    }

    // Show unknown arguments
    for (Cli_Arg *arg = cli->arg_rest; arg; arg = arg->next) {
        GUARD(!arg->used);
        fmt_s(f, "Unknown argument: ");
        fmt_s(f, arg->value);
        fmt_s(f, "\n");
    }

    {
        // Header
        u32 c = fmt_cursor(f);
        fmt_pad(f, c, ' ', pad1 / 2 - 3, 0);
        fmt_s(f, "Name");
        fmt_pad(f, c, ' ', pad1, 0);
        fmt_s(f, "|");
        fmt_pad(f, c, ' ', pad1 / 2 + pad2 / 2 - 3, 0);
        fmt_s(f, "Info");
        fmt_pad(f, c, ' ', pad2, 0);
        if (has_command) {
            fmt_s(f, "|");
            fmt_pad(f, c, ' ', pad2 / 2 + pad3 / 2 - 3, 0);
            fmt_s(f, "Value");
        }
        fmt_s(f, "\n");
    }
    {
        u32 c = fmt_cursor(f);
        fmt_pad(f, c, '-', pad1, 0);
        fmt_s(f, "|");
        fmt_pad(f, c, '-', pad2, 0);
        if (has_command) {
            fmt_s(f, "|");
            fmt_pad(f, c, '-', pad3, 0);
        }
        fmt_s(f, "\n");
    }

    for (Cli_Command *cmd = cli->command_first; cmd; cmd = cmd->next) {
        GUARD(cmd->match || !has_command);

        {
            u32 c = fmt_cursor(f);
            fmt_s(f, "  ");
            fmt_s(f, cmd->name);
            fmt_pad(f, c, ' ', pad1, 0);
            fmt_s(f, "| ");
            fmt_s(f, cmd->info);
            if (has_command) {
                fmt_pad(f, c, ' ', pad2, 0);
                fmt_s(f, "|");
            }
            fmt_s(f, "\n");
        }

        for (Cli_Value *val = cmd->value_first; val; val = val->next) {
            u32 c = fmt_cursor(f);
            fmt_s(f, "      ");
            fmt_s(f, val->name);
            fmt_pad(f, c, ' ', pad1, 0);
            fmt_s(f, "| ");
            fmt_s(f, val->info);
            if (has_command) {
                fmt_pad(f, c, ' ', pad2, 0);
                fmt_s(f, "|");
                if (val->match) {
                    fmt_s(f, " \"");
                    fmt_s(f, val->match);
                    fmt_s(f, "\"");
                } else {
                    fmt_s(f, " (MISSING)");
                }
            }
            fmt_s(f, "\n");
        }

        for (Cli_Flag *flag = cmd->flag_first; flag; flag = flag->next) {
            u32 c = fmt_cursor(f);
            fmt_s(f, "      ");
            fmt_s(f, flag->name);
            fmt_pad(f, c, ' ', pad1, 0);
            fmt_s(f, "| ");
            fmt_s(f, flag->info);
            if (has_command) {
                fmt_pad(f, c, ' ', pad2, 0);
                fmt_s(f, "|");
                if (flag->match) {
                    fmt_pad(f, c, ' ', pad2, 0);
                    fmt_s(f, " true");
                }
            }
            fmt_s(f, "\n");
        }
    }
}

static void cli_help(Cli *cli) {
    bool need_help = 0;
    bool full_help = 0;

    // Explicit help command
    bool help_command = cli_command(cli, "help", "Show Help");
    if (help_command) {
        need_help = 1;
        full_help = 1;
    }

    if (cli->show_help) {
        need_help = 1;
    }

    // Unmatched command
    if (!cli->has_match) {
        need_help = 1;
        full_help = 1;
    }

    // Unused arguments
    for (Cli_Arg *arg = cli->arg_rest; arg; arg = arg->next) {
        if (arg->used) continue;
        need_help = 1;
        break;
    }

    // Unmatched input
    for (Cli_Command *cmd = cli->command_first; cmd; cmd = cmd->next) {
        if (!cmd->match) continue;
        for (Cli_Value *val = cmd->value_first; val; val = val->next) {
            if (val->match) continue;
            need_help = 1;
            break;
        }
    }

    if (!need_help) return;
    cli_showhelp(cli, full_help);
    os_exit(help_command ? 0 : 1);
}

static void cli_test(Test *test) {
    Cli *cli = cli_from(test->mem, 5, (char *[]){"cli_test", "hello", "--xx", "world", "--y"});
    TEST(cli_command(cli, "test", "Test") == 0);
    TEST(cli_command(cli, "world", "World") == 0);
    TEST(cli_flag(cli, "--xx", "X") == 0);
    TEST(cli_flag(cli, "--y", "Y") == 0);
    TEST(cli_flag(cli, "--zz", "Z") == 0);
    TEST(cli_command(cli, "hello", "Hello") == 1);
    TEST(cli_flag(cli, "--xx", "X") == 1);
    TEST(cli_flag(cli, "--y", "Y") == 1);
    TEST(cli_flag(cli, "--zz", "Z") == 0);
    TEST(cli_command(cli, "hi", "Hi") == 0);
    TEST(cli->has_match == 1);
}
