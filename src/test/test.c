#include "build/cli.h"
#include "lib/math_test.h"
#include "lib/os_main.h"
#include "lib/str_test.h"

static void lib_test(void) {
    // test_part();
    // test_text();
}

static void os_main(void) {
    Test *test = test_begin();
    // lib_test();
    str_test(test);
    math_test(test);
    cli_test(test);
    test_end(test);
}
