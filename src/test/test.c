#include "lib/math.h"
#include "lib/midi.h"
#include "lib/os_main.h"
#include "lib/str_mem.h"
#include "qfn/game.h"
#include "lang/lang.h"

static void gfx_audio_callback(u32 sample_count, v2 *sample_list) {}

static void lib_test(void) {
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
}

static void os_main(void) {
    lib_test();
    lang_test();
    fmt_s(G->fmt, "All tests OK!\n");
    os_exit(0);
}
