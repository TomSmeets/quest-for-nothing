#include "lib/math.h"
#include "lib/midi.h"
#include "lib/os_main.h"
// #include "lib/part.h"
#include "lib/str_mem.h"
// #include "lib/text.h"

static void os_main(void) {
    fmt_s(G->fmt, "EXP(1) = ");
    fmt_f(G->fmt, f_exp(1));
    fmt_s(G->fmt, "\n");

    fmt_s(G->fmt, "EXP(0) = ");
    fmt_f(G->fmt, f_exp(0));
    fmt_s(G->fmt, "\n");

    test_str();
    // test_part();
    // test_text();
    test_midi();
    test_math();

    fmt_s(G->fmt, "All tests OK!\n");
    os_exit(0);
}
