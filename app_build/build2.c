#include "clang.h"
#include "fmt.h"
#include "os_main.h"

typedef struct Cli Cli;
struct Cli {
    u32 ix;
    u32 argc;
    char **argv;

    u32 option_count;
    char *option_list[64][2];
};

static void cli_debug(Cli *cli, Fmt *fmt) {
    for (u32 i = 0; i < cli->argc; ++i) {
        fmt_su(fmt, "[", i, "] ");
        fmt_s(fmt, cli->argv[i]);
        fmt_s(fmt, "\n");
    }
}

static char *cli_read(Cli *cli) {
    if (cli->ix == cli->argc) return 0;
    return cli->argv[cli->ix];
}

static char *cli_next(Cli *cli) {
    if (cli->ix == cli->argc) return 0;
    return cli->argv[cli->ix++];
}

static bool cli_flag(Cli *cli, char *name, char *description) {
    if (cli->option_count < array_count(cli->option_list)) {
        cli->option_list[cli->option_count][0] = name;
        cli->option_list[cli->option_count][1] = description;
        cli->option_count++;
    }

    char *arg = cli_read(cli);
    if (!arg) return false;

    if (str_eq(arg, name)) {
        cli->option_count = 0;
        cli_next(cli);
        return true;
    }

    return false;
}

static char *cli_value(Cli *cli, char *name, char *description) {
    cli->option_count = 0;
    return cli_next(cli);
}

static void cli_show_usage(Cli *cli, Fmt *fmt) {
    u32 pos = 0;
    u32 len = 0;
    for (u32 i = 0; i < cli->argc; ++i) {
        if (i == cli->ix) {
            pos = fmt_cursor(fmt);
            len = str_len(cli->argv[i]);
        }

        fmt_s(fmt, cli->argv[i]);
        fmt_s(fmt, " ");
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
        fmt_pad(fmt, cur, ' ', 12, false);
        fmt_s(fmt, cli->option_list[i][1]);
        fmt_s(fmt, "\n");
    }
}

struct App {};

static void build_show_usage(void) {

    fmt_s(G->fmt, "Usage: ./build <ACTION> [...]\n");
    fmt_s(G->fmt, "  build <INPUT> <OUTPUT> [MODE] [PLATFORM]\n");
    fmt_s(G->fmt, "  run <INPUT> [MODE]\n");
}

static void os_main(void) {
    Memory *tmp = mem_new();

    Cli cli = {
        .ix = 0,
        .argc = G->os->argc,
        .argv = G->os->argv,
    };

    char *exe_name = cli_next(&cli);

    if (cli_flag(&cli, "run", "Run an application with dynamic hot reloading")) {
    } else if (cli_flag(&cli, "format", "Run code formatter")) {
        assert(os_system("clang-format --verbose -i */*.{h,c}"), "Format failed!");
        os_exit(0);
    } else if (cli_flag(&cli, "build", "Build an executable")) {
        Clang_Options opts = {};
        opts.input_path = cli_value(&cli, "Input", "Source Input Path");
        assert(opts.input_path, "Missing Input file");

        opts.output_path = cli_value(&cli, "Output", "Execurtable Output Path");
        assert(opts.output_path, "Missing Output file");

        while (cli_read(&cli)) {
            if (0) {
            } else if (cli_flag(&cli, "linux", "Compile for Linux")) {
                opts.platform = Platform_Linux;
            } else if (cli_flag(&cli, "windows", "Compile for Windows")) {
                opts.platform = Platform_Windows;
            } else if (cli_flag(&cli, "wasm", "Compile for Webassembly")) {
                opts.platform = Platform_Wasm;
            } else if (cli_flag(&cli, "release", "Compile in Release Mode")) {
                opts.release = true;
            } else if (cli_flag(&cli, "dynamic", "Create a dynamic executable (a .dll/.so file)")) {
                opts.dynamic = true;
            } else {
                cli_show_usage(&cli, G->fmt);
                os_exit(1);
            }
        }

        clang_compile(tmp, opts);
    } else {
        cli_show_usage(&cli, G->fmt);
        os_exit(1);
    }

    os_exit(0);
}
