#pragma once
#include "build/clang.h"
#include "build/cli.h"
#include "build/hot.h"
#include "build/include_graph.h"
#include "build/watch.h"
#include "lib/fmt.h"

// Read command line options for build targets
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

#define TYPEDEF_STRUCT(NAME) typedef struct NAME NAME;

TYPEDEF_STRUCT(Build);
TYPEDEF_STRUCT(Build_Source);

// Source paths
struct Build_Source {
    String path;
    Build_Source *next;
};

struct Build {
    // Permanent memory
    Memory *mem;

    // Memory that is reset after each frame
    Memory *tmp;

    // Source directories
    Build_Source *sources;
};

static Build *build_new(Memory *mem) {
    Build *build = mem_struct(mem, Build);
    build->mem = mem;
    build->tmp = mem_new();
    return build;
}

static void build_add_source(Build *build, String path) {
    Build_Source *src = mem_struct(build->mem, Build_Source);
    src->path = path;
    LIST_PUSH(build->sources, src, next);
}

// Code formatter option
// Returns true if the argument is matched
static bool build_format(Build *build, Cli *cli) {
    if (!cli_flag(cli, "format", "Run code formatter")) return false;

    Fmt *cmd = fmt_memory(build->tmp);
    fmt_str(cmd, S("clang-format --verbose -i --"));
    for(Build_Source *src = build->sources; src; src = src->next) {
        fmt_str(cmd, S(" "));
        fmt_str(cmd, src->path);
        fmt_str(cmd, S("/**.{h,c}"));
    }

    bool result = os_system(S("clang-format --verbose -i {tlib/src,src}/*/*.{h,c}"));
    assert(result, "Format failed!");
    os_exit(0);
    return true;
}
