#pragma once
#include "mem.h"
#include "fmt.h"
#include "os.h"

// Get a new absolute path to a temporary file
static char *fs_tmpfile(Memory *mem, char *name) {
    Fmt *f = fmt_memory(mem);
    fmt_s(f, "/tmp/tmp-");
    fmt_u(f, os_rand());
    fmt_s(f, "_");
    fmt_s(f, name);
    return fmt_close(f);
}

static void os_exec(char **cmd) {
}


