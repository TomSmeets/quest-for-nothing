// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// build.c - Build tool for runing and building applications
#include "lib/cli2.h"
#include "lib/fmt.h"
#include "lib/mem.h"
#include "lib/os_main.h"
#include "lib/types.h"

// ./build <command> [Input0] --option1  [input1] --option2
typedef enum {
    Argument_Word,  // [WORD]
    Argument_Value, // =value
    Argument_Short, // -f
    Argument_Long,  // --flag
} Argument_Type;

TYPEDEF_STRUCT(Argument);
struct Argument {
    String name;
    Argument_Type type;
    bool is_used;
    bool is_expected;
    Argument *next;
};

static Argument *argument_new(Memory *mem, u32 argc, char **argv) {
    Argument *first = 0;
    Argument *last = 0;
    bool no_flags = false;
    for (u32 i = 1; i < argc; ++i) {
        if (strz_eq(argv[i], "--")) {
            no_flags = true;
            continue;
        }

        Argument *arg = mem_struct(mem, Argument);
        arg->name = str_from(argv[i]);
        arg->type = Argument_Word;

        if (!no_flags) {
            if (str_starts_with(arg->name, S("--"))) {
                arg->type = Argument_Long;
                arg->name = str_drop_start(arg->name, 2);
            } else if (str_starts_with(arg->name, S("-"))) {
                arg->type = Argument_Short;
                arg->name = str_drop_start(arg->name, 1);
            }
        }

        LIST_APPEND(first, last, arg);
    }
    return first;
}

static void argument_print(Fmt *fmt, Argument *args) {
    for (Argument *arg = args; arg; arg = arg->next) {
        bool first = arg == args;
        u32 cursor = fmt_cursor(fmt);
        if (!first) fmt_s(fmt, " ");
        if (arg->type == Argument_Short) fmt_s(fmt, "[s]");
        if (arg->type == Argument_Long) fmt_s(fmt, "[l]");
        if (arg->type == Argument_Value) fmt_s(fmt, "[v]");
        if (arg->type == Argument_Word) fmt_s(fmt, "[w]");
        if (arg->is_used) fmt_s(fmt, "[u]");
        fmt_str(fmt, arg->name);
    }
    fmt_s(fmt, "\n");
}

static void argument_help(Fmt *fmt, Argument *args) {
    for (Argument *arg = args; arg; arg = arg->next) {
        if (arg->is_expected) {
            fmt_s(fmt, "Expecting: ");
            fmt_str(fmt, arg->name);
            fmt_s(fmt, "\n");
        } else if (!arg->is_used) {
            if (arg->type == Argument_Short) {
                fmt_s(fmt, "Unknown flag: -");
                fmt_str(fmt, arg->name);
                fmt_s(fmt, "\n");
            }

            if (arg->type == Argument_Long) {
                fmt_s(fmt, "Unknown flag: --");
                fmt_str(fmt, arg->name);
                fmt_s(fmt, "\n");
            }

            if (arg->type == Argument_Word) {
                fmt_s(fmt, "Unknown argument: ");
                fmt_str(fmt, arg->name);
                fmt_s(fmt, "\n");
            }

            if (arg->type == Argument_Value) {
                fmt_s(fmt, "Unknown value: ");
                fmt_str(fmt, arg->name);
                fmt_s(fmt, "\n");
            }
            if (arg->type == Argument_Word) break;
        }
    }
}

// Expand -abc args into -a -b -c
static void argument_expand_short(Argument *args) {
    for (Argument *arg = args; arg; arg = arg->next) {
        if (arg->type != Argument_Short) continue;

        while (arg->name.len > 1) {
            Argument *new = mem_struct(G->mem, Argument);
            new->name = str_slice(arg->name, arg->name.len - 1, 1);
            new->type = Argument_Short;
            arg->name = str_drop_end(arg->name, 1);
            new->next = arg->next;
            arg->next = new;
        }
    }
}

// Expand --key=Value into --key value
static void argument_expand_long(Argument *args) {
    for (Argument *arg = args; arg; arg = arg->next) {
        if (arg->type != Argument_Long) continue;

        u32 ix = str_find(arg->name, '=');
        if (ix == arg->name.len) continue;

        Argument *new = mem_struct(G->mem, Argument);
        new->name = str_slice(arg->name, ix + 1, arg->name.len - ix - 1);
        new->type = Argument_Value;

        arg->name = str_slice(arg->name, 0, ix);
        new->next = arg->next;
        arg->next = new;
    }
}
struct App {
    Cli *cli;
};

static void build_build(Cli *cli) {
    bool active = cli_command(cli, "build", "Build executable");
    char *input = cli_value(cli, "[INPUT]", "Input C file");
    char *output = cli_value(cli, "[OUTPUT]", "Output file");
    bool plat_linux = cli_flag(cli, "--linux", "For Linux");
    bool plat_windows = cli_flag(cli, "--windows", "For Windows");
    bool plat_web = cli_flag(cli, "--web", "For Web Assembly");
    bool opt_release = cli_flag(cli, "--release", "Relase mode");
    if (!active) return;
}

static void build_run(Cli *cli) {
    bool active = cli_command(cli, "run", "Run source file");
    bool opt_release = cli_flag(cli, "--release", "Build in relase mode");
    if (!active) return;
}

static void build_format(Cli *cli) {
    bool build = cli_command(cli, "format", "Format source");
    if (!build) return;
}

static void os_main(void) {
    Cli *cli = cli_new();
    build_build(cli);
    build_run(cli);
    build_format(cli);
    cli_help(cli);
    os_exit(0);
}
