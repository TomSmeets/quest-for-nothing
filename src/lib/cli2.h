#include "lib/fmt.h"
#include "lib/mem.h"

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
    char *arg_command;
    Cli_Arg *arg_rest;

    // Help info
    Cli_Command *command_first;
    Cli_Command *command_last;
} Cli;

static bool cli_command(Cli *cli, char *name, char *info) {
    Cli_Command *cmd = mem_struct(cli->mem, Cli_Command);
    cmd->name = name;
    cmd->info = info;
    cmd->match = strz_eq(name, cli->arg_command);
    LIST_APPEND(cli->command_first, cli->command_last, cmd);
    return cmd->match;
}

static bool cli_flag(Cli *cli, char *name, char *info) {
    Cli_Command *cmd = cli->command_last;
    assert0(cmd);

    Cli_Flag *flag = mem_struct(cli->mem, Cli_Flag);
    flag->name = name;
    flag->info = info;
    LIST_APPEND(cmd->flag_first, cmd->flag_last, flag);

    if(cmd->match) {
    for(Cli_Arg *arg = cli->arg_rest; arg; arg = arg->next) {
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

    if(cmd->match) {
    for(Cli_Arg *arg = cli->arg_rest; arg; arg = arg->next) {
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
    for(u32 i = 0; i < argc; ++i) {
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
    if (argc > 1) cli->arg_command = argv[1];
    if (argc > 2) cli->arg_rest = arg_parse(mem, argc - 2, argv + 2);
    return cli;
}

static Cli *cli_new(void) {
    return cli_from(G->tmp, G->argc, G->argv);
}

// Check for the 'help' subcommand and handle no matches
// Call at the end for correct handling
static void cli_help(Cli *cli) {
    Fmt *f = G->fmt;


    fmt_s(f, cli->arg_command);
    for (Cli_Arg *arg = cli->arg_rest; arg; arg = arg->next) {
        fmt_s(f, " ");
        fmt_s(f, arg->value);
    }
    fmt_s(f, "\n");

    for (Cli_Arg *arg = cli->arg_rest; arg; arg = arg->next) {
        GUARD(!arg->used);
        fmt_s(f, "Unknown argument: ");
        fmt_s(f, arg->value);
        fmt_s(f, "\n");
    }

    fmt_s(f, "\n");
    fmt_s(f, "Usage:\n");

    for(Cli_Command *cmd = cli->command_first; cmd; cmd = cmd->next) {
        fmt_s(f, "  ");
        fmt_s(f, cmd->name);
        fmt_s(f, " | ");
        fmt_s(f, cmd->info);

        if(cmd->match) fmt_s(f, " (MATCH)");

        fmt_s(f, "\n");

        for(Cli_Value *val = cmd->value_first; val; val = val->next) {
            fmt_s(f, "    ");
            fmt_s(f, val->name);
            fmt_s(f, " | ");
            fmt_s(f, val->info);
            if(val->match) fmt_s(f, " (MATCH)");
            fmt_s(f, "\n");
        }

        for(Cli_Flag *flag = cmd->flag_first; flag; flag = flag->next) {
            fmt_s(f, "    ");
            fmt_s(f, flag->name);
            fmt_s(f, " | ");
            fmt_s(f, flag->info);
            if(flag->match) fmt_s(f, " (MATCH)");
            fmt_s(f, "\n");
        }
    }
}


