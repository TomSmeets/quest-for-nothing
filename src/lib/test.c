#include "lib/midi.h"
#include "lib/os_main.h"
#include "lib/part.h"
#include "lib/str_mem.h"
#include "lib/text.h"

static void os_main(void) {
    test_str();
    test_part();
    test_text();
    test_midi();

    fmt_s(G->fmt, "All tests OK!\n");
    os_exit(0);
}
