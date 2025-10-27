#include "lib/mem.h"
#include "lib/fmt.h"

TYPEDEF_STRUCT(Cli_Option);
struct Cli_Option{
    char *name;
    char *info;
    Cli_Option *next;

    Cli_Option *flags;
    Cli_Option *values;
};

typedef struct {
    Memory *mem;

    u32 argc;
    char **argv;
    bool *used;

    Cli_Option *options;
    Cli_Option *options_last;
    bool has_match;
} Cli;


// Construct a new command line argument parser
// using argc/argv and global allocator
static Cli *cli_new(void) {
    Memory *mem = G->mem;
    Cli *cli = mem_struct(mem, Cli);
    cli->mem   = mem;
    cli->argc = G->argc;
    cli->argv = G->argv;
    cli->used = mem_array_zero(mem, bool, cli->argc);
    return cli;
}


// Check for a top-level subcommand and add to documentation
static bool cli_command(Cli *cli, char *name, char *info) {
    Cli_Option *opt = mem_struct(cli->mem, Cli_Option);
    opt->name = name;
    opt->info = info;
    LIST_APPEND(cli->options, cli->options_last, opt);

    if (cli->has_match) return false;
    if (cli->argc < 2) return false;
    if (!strz_eq(cli->argv[1], name)) return false;
    cli->has_match = true;
    return true;
}

static char *cli_value(Cli *cli, char *name, char *info) {
    Cli_Option *opt = mem_struct(cli->mem, Cli_Option);
    opt->name = name;
    opt->info = info;
    LIST_PUSH(cli->options_last->values, opt);
    return 0;
}

static bool cli_flag(Cli *cli, char *name, char *info) {
    Cli_Option *opt = mem_struct(cli->mem, Cli_Option);
    opt->name = name;
    opt->info = info;
    LIST_PUSH(cli->options_last->flags, opt);
    return 0;
}

// Check for the 'help' subcommand and handle no matches
// Call at the end for correct handling
static void cli_help(Cli *cli) {
    bool active = cli_command(cli, "help", "Show this help");
    if(!active && cli->has_match) return;

    for(Cli_Option *opt = cli->options; opt; opt = opt->next) {
        u32 cursor = fmt_cursor(G->fmt);
        fmt_s(G->fmt, "    ");
        fmt_s(G->fmt, opt->name);
        fmt_s(G->fmt, " ");
        fmt_pad(G->fmt, cursor, ' ', 24, false);
        fmt_s(G->fmt, "| ");
        fmt_s(G->fmt, opt->info);
        fmt_s(G->fmt, "\n");

        for(Cli_Option *value = opt->values; value; value = value ->next) {
            u32 cursor = fmt_cursor(G->fmt);
            fmt_s(G->fmt, "    ");
            fmt_s(G->fmt, "    ");
            fmt_s(G->fmt, value->name);
            fmt_s(G->fmt, " ");
            fmt_pad(G->fmt, cursor, ' ', 24, false);
            fmt_s(G->fmt, "| ");
            fmt_s(G->fmt, value->info);
            fmt_s(G->fmt, "\n");
        }

        for(Cli_Option *flag = opt->flags; flag; flag = flag ->next) {
            u32 cursor = fmt_cursor(G->fmt);
            fmt_s(G->fmt, "    ");
            fmt_s(G->fmt, "    ");
            fmt_s(G->fmt, flag->name);
            fmt_s(G->fmt, " ");
            fmt_pad(G->fmt, cursor, ' ', 24, false);
            fmt_s(G->fmt, "| ");
            fmt_s(G->fmt, flag->info);
            fmt_s(G->fmt, "\n");
        }
        fmt_s(G->fmt, "\n");
    }
}
