// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// build.c - Build tool for runing and building applications
#include "lib/fmt.h"
#include "lib/mem.h"
#include "lib/types.h"
#include "lib/os_main.h"

TYPEDEF_STRUCT(Cli_Option);

// ./build <command> [Input0] --option1  [input1] --option2
//
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
    for(u32 i = 1; i < argc; ++i) {
        if(strz_eq(argv[i], "--")) {
            no_flags = true;
            continue;
            }

        Argument *arg = mem_struct(mem, Argument);
        arg->name = str_from(argv[i]);
            arg->type = Argument_Word;

            if(!no_flags) {
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
    for(Argument *arg = args; arg; arg = arg->next) {
        bool first = arg == args;
        u32 cursor = fmt_cursor(fmt);
        if(!first) fmt_s(fmt, " ");
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
    for(Argument *arg = args; arg; arg = arg->next) {
        if(arg->is_expected) {
            fmt_s(fmt, "Expecting: ");
            fmt_str(fmt, arg->name);
            fmt_s(fmt, "\n");
        } else if(!arg->is_used) {
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
            if(arg->type == Argument_Word) break;
        }
    }
}

struct Cli_Option {
    u32 index;
    String name;
    String info;
    Cli_Option *next;
    Cli_Option *parent;
};

typedef struct {
    Memory *mem;
    Argument *args;
} Cli;

struct App {
    Cli *cli;
};

static Cli *cli_new(void) {
    Memory *mem = G->mem;
    Cli *cli = mem_struct(mem, Cli);
    cli->mem = mem;
    cli->args =argument_new(mem, G->argc, G->argv);
    return cli;
}


static void cli_begin(Cli *cli, char *name, char *info) { }
static void cli_end(Cli *cli) { }

static bool cli_flag(Cli *cli, char *name, char *info) {
    cli_begin(cli, name, info);
    cli_end(cli);
    return false;
}

static String cli_next(Cli *cli, char *name, char *info) {
    cli_begin(cli, name, info);
    cli_end(cli);
    return S0;
}


// 0. Lex     ./build run -la --test --hello=4
// 1. Expand ./build run  -l -a
//

static void argument_expand_short(Argument *args) {
    for(Argument *arg = args; arg; arg = arg->next) {
        if(arg->type != Argument_Short) continue;

        while(arg->name.len > 1) {
            Argument *new = mem_struct(G->mem, Argument);
            new->name = str_slice(arg->name, arg->name.len - 1, 1);
            new->type = Argument_Short;
            arg->name = str_drop_end(arg->name, 1);
            new->next = arg->next;
            arg->next = new;
        }
    }
}

static void argument_expand_long(Argument *args) {
    for(Argument *arg = args; arg; arg = arg->next) {
        if(arg->type != Argument_Long) continue;

        u32 ix = str_find(arg->name, '=');
        if(ix == arg->name.len) continue;

        Argument *new = mem_struct(G->mem, Argument);
        new->name = str_slice(arg->name, ix + 1, arg->name.len - ix - 1);
        new->type = Argument_Value;

        arg->name = str_slice(arg->name, 0, ix);
        new->next = arg->next;
        arg->next = new;
    }
}

static String argument_word(Argument *args, String info) {
    for(Argument *arg = args; arg; arg = arg->next) {
        if(arg->is_used) continue;
        if(arg->type != Argument_Word) continue;
        arg->is_used = true;
        return arg->name;
    }

    for(Argument *arg = args; arg; arg = arg->next) {
        if(arg->next) continue;
        Argument *new = mem_struct(G->mem, Argument);
        new->name = info;
        new->type = Argument_Value;
        new->is_expected = true;
        new->is_used     = true;
        arg->next = new;
        break;
    }

    return S0;
}

static bool argument_match(Argument *args, String match) {
    for(Argument *arg = args; arg; arg = arg->next) {
        if(arg->is_used) continue;
        if(arg->type != Argument_Word) continue;
        if(!str_eq(arg->name, match)) break;
        arg->is_used = true;
        return true;
    }
    return false;
}

static bool argument_flag(Argument *args, String name_short, String name_long) {
    for(Argument *arg = args; arg; arg = arg->next) {
        if(arg->is_used) continue;
        if(!(arg->type == Argument_Short && str_eq(arg->name, name_short))) continue;
        if(!(arg->type == Argument_Long  && str_eq(arg->name, name_long)))  continue;
        arg->is_used = true;
        return true;
    }
    return false;
}

static void os_main(void) {
    Argument *args = argument_new(G->mem, G->argc, G->argv);
    argument_print(G->fmt, args);
    argument_expand_short(args);
    argument_expand_long(args);
    argument_print(G->fmt, args);

    if(argument_match(args, S("build"))) {
        String src = argument_word(args, S("Source"));
        String dst = argument_word(args, S("Output"));
        bool release = argument_flag(args, S("r"), S("release"));
        bool linux = argument_flag(args, S("l"), S("linux"));
        bool windows = argument_flag(args, S("w"), S("windows"));
        bool wasm = argument_flag(args, S("j"), S("wasm"));
    }

    // for (;;) {
    //     String word = argument_word(args);
    //     if (word.len == 0) break;
    //     fmt_s(G->fmt, "WORD: ");
    //     fmt_str(G->fmt, word);
    //     fmt_s(G->fmt, "\n");
    // }

    argument_print(G->fmt, args);
    argument_help(G->fmt, args);
    os_exit(0);
}
