#include "build/cli.h"
#include "lib/chunk_test.h"
#include "lib/math_test.h"
#include "lib/midi.h"
#include "lib/os_main.h"
#include "lib/part.h"
#include "lib/str_test.h"
#include "lib/text.h"

static void os_main(void) {
    Test *test = test_begin();

    chunk_test(test);
    str_test(test);
    part_test(test);
    // text_test(test);
    // midi_test(test);
    math_test(test);
    cli_test(test);

    test_end(test);
}
