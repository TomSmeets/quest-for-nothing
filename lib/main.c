// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// main.c - Example main for lib/
#include "fmt.h"
#include "os_main.h"

static void os_main(void) {
    fmt_s(G->fmt, "Hello World!\n");
}
