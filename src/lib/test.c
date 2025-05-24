#include "os_main.h"
#include "part.h"
#include "str_mem.h"
#include "text.h"
#include "midi.h"

static void os_main(void) {
    test_str();
    test_part();
    test_text();
    test_midi();

    fmt_s(G->fmt, "All tests OK!\n");
    os_exit(0);
}
