// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// build.c - Build tool for runing and building applications
#include "lib/cli2.h"
#include "lib/os_main.h"

struct App {
    void *x;
};

static void os_main(void) {
    Cli *cli = cli_new();
    cli_command(cli, "build", "Build Application");
    cli_flag(cli, "--linux", "For Linux");
    cli_flag(cli, "--windows", "For Windows");
    cli_flag(cli, "--wasm", "For WASM");
    cli_flag(cli, "--release", "In Release Mode");
    cli_value(cli, "[SOURCE]", "Source file");
    cli_value(cli, "[OUTPUT]", "Executable file");

    cli_command(cli, "run", "Run   Application");
    cli_value(cli, "[SOURCE]", "Source file");
    cli_flag(cli, "--release", "In Release Mode");

    cli_help(cli);
    os_exit(0);
}
