#include "clang.h"
#include "fmt.h"
#include "hot.h"
#include "os_main.h"
#include "watch.h"

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
    cli->option_count = 1;
    cli->option_list[0][0] = name;
    cli->option_list[0][1] = description;
    return cli_next(cli);
}

static void cli_show_usage(Cli *cli, Fmt *fmt) {
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
    for (u32 i = 0; i < cli->option_count; ++i) {
        fmt_s(fmt, "    ");
        u32 cur = fmt_cursor(fmt);
        fmt_s(fmt, cli->option_list[i][0]);
        fmt_pad(fmt, cur, ' ', 12, false);
        fmt_s(fmt, cli->option_list[i][1]);
        fmt_s(fmt, "\n");
    }
}

struct App {
    Watch watch;

    // Build
    bool do_build;
    Clang_Options build_opts;

    // Hot
    bool do_hot;
    Hot *hot;
    char *hot_path;
    u32 hot_argc;
    char **hot_argv;
    Fmt hot_output_fmt;

    // First time?
    bool first;
};

static bool build_read_opts(Cli *cli, Clang_Options *opts) {
    opts->input_path = cli_value(cli, "<Input>", "Source Input Path");
    if (!opts->input_path) return false;

    opts->output_path = cli_value(cli, "<Output>", "Execurtable Output Path");
    if (!opts->output_path) return false;

    while (cli_read(cli)) {
        if (0) {
        } else if (cli_flag(cli, "linux", "Compile for Linux")) {
            opts->platform = Platform_Linux;
        } else if (cli_flag(cli, "windows", "Compile for Windows")) {
            opts->platform = Platform_Windows;
        } else if (cli_flag(cli, "wasm", "Compile for Webassembly")) {
            opts->platform = Platform_Wasm;
        } else if (cli_flag(cli, "release", "Compile in Release Mode")) {
            opts->release = true;
        } else if (cli_flag(cli, "dynamic", "Create a dynamic executable (a .dll/.so file)")) {
            opts->dynamic = true;
        } else {
            return false;
        }
    }
    return true;
}

static void os_main(void) {
    Memory *tmp = mem_new();

    if (!G->app) {
        Memory *mem = mem_new();
        G->app = mem_struct(mem, App);

        Cli cli = {
            .ix = 0,
            .argc = G->os->argc,
            .argv = G->os->argv,
        };

        // Exe name
        cli_next(&cli);

        if (cli_flag(&cli, "run", "Run an application with dynamic hot reloading")) {
            char *input_path = cli_value(&cli, "<INPUT>", "Input file");

            G->app->hot_path = input_path;
            G->app->hot_argv = cli.argv + cli.ix - 1;
            G->app->hot_argc = cli.argc - cli.ix + 1;
            G->app->hot = hot_new(mem, G->app->hot_argc, G->app->hot_argv);
            G->app->do_hot = true;
        } else if (cli_flag(&cli, "format", "Run code formatter")) {
            assert(os_system("clang-format --verbose -i */*.{h,c}"), "Format failed!");
        } else if (cli_flag(&cli, "build", "Build an executable")) {
            Clang_Options opts = {};
            if (!build_read_opts(&cli, &opts)) {
                cli_show_usage(&cli, G->fmt);
                os_exit(1);
            }
            clang_compile(tmp, opts);
        } else if (cli_flag(&cli, "watch", "Build an executable and watch changes")) {
            if (!build_read_opts(&cli, &G->app->build_opts)) {
                cli_show_usage(&cli, G->fmt);
                os_exit(1);
            }
            G->app->do_build = true;
        } else {
            cli_show_usage(&cli, G->fmt);
            os_exit(1);
        }

        if (!G->app->do_build && !G->app->do_hot) {
            os_exit(0);
        }

        G->app->first = 1;

        // We keep going, so init watch
        watch_add(&G->app->watch, "app_build");
        watch_add(&G->app->watch, "src");
        watch_add(&G->app->watch, "lib");
    }

    if (watch_check(&G->app->watch) || G->app->first) {
        G->app->first = false;
        if (G->app->do_build) clang_compile(tmp, G->app->build_opts);
        if (G->app->do_hot) {
            Fmt *fmt = &G->app->hot_output_fmt;
            fmt->used = 0;
            fmt_su(fmt, "out/hot_", os_time(), ".so");
            char *out_path = fmt_close(fmt);
            fmt_ss(G->fmt, "OUT: ", out_path, "\n");
            if (clang_compile(
                    tmp,
                    (Clang_Options){
                        .input_path = G->app->hot_path,
                        .output_path = out_path,
                        .dynamic = true,
                    }
                )) {
                hot_load(G->app->hot, out_path);
            };
        }
    }

    if (G->app->do_hot) {
        hot_update(G->app->hot);
    } else {
        G->os->sleep_time = 100 * 1000;
    }

    mem_free(tmp);
}
