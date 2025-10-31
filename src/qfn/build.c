// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// build.c - Build tool for runing and building applications
#include "lib/cli.h"
#include "lib/os_main.h"

struct App {
    void *x;
};

static void cmd_build(Cli *cli) {
    bool match = cli_command(cli, "build", "Build Application");

    char *input = cli_value(cli, "<Input>", "Input Source file");
    char *output = cli_value(cli, "<Output>", "Output Executable file");

    bool linux = cli_flag(cli, "--linux", "For Linux");
    bool windows = cli_flag(cli, "--windows", "For Windows");
    bool wasm = cli_flag(cli, "--wasm", "For WASM");
    bool release = cli_flag(cli, "--release", "In Release Mode");
    bool watch = cli_flag(cli, "--watch", "Watch for changes");
    bool all = cli_flag(cli, "--all", "Build all platforms");
}

static void cmd_run(Cli *cli) {
    bool match = cli_command(cli, "run", "Run Application with Hot reloading");
    char *input = cli_value(cli, "<Input>", "Input Source file");
    bool release = cli_flag(cli, "--release", "In Release Mode");
}

static void os_main(void) {
    Cli *cli = cli_new();
    cmd_run(cli);
    cmd_build(cli);
    cli_help(cli);
    os_exit(0);
}
