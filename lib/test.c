#include "os_main.h"
#include "str_mem.h"
#include "text.h"

static void os_main(void) {
    test_str();
    test_text();

    fmt_s(G->fmt, "All tests OK!\n");
    os_exit(0);
}
