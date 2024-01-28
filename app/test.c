#include "fmt.h"
#include "os_generic.h"


static void test_str_eq_imp(char *comp, char *test, bool eq) {
    os_print(test);
    os_print("\t== ");
    os_print(comp);
    os_print("\n");

    if(eq) {
        assert(str_eq(comp, test));
    } else {
        assert(!str_eq(comp, test));
    }
}

static void test_str_eq(char *comp, char *test) { test_str_eq_imp(comp, test, 1); }
static void test_str_not_eq(char *comp, char *test) { test_str_eq_imp(comp, test, 0); }

void *main_init(int argc, char **argv) {
    mem m = {};
    test_str_eq("Hello World", "Hello World");
    test_str_not_eq("Hello World", "Hello");
    test_str_eq(fmt(&m, "Hello World"), "Hello World");
    test_str_eq(fmt(&m, "n = %u",  1337), "n = 1337");
    test_str_not_eq(fmt(&m, "n = %u", -1337), "n = -1337");

    test_str_eq(fmt(&m, "n = %i",  1337), "n = 1337");
    test_str_eq(fmt(&m, "n = %i", -1337), "n = -1337");

    test_str_eq(fmt(&m, "n = %f", 1337.0f), "n = 1337.000");
    test_str_eq(fmt(&m, "n = %f", 1337.0),  "n = 1337.000");
    test_str_eq(fmt(&m, "n = %f",  0.001f), "n = 0.001");
    test_str_eq(fmt(&m, "n = %f", -0.001f), "n = -0.001");
    test_str_eq(fmt(&m, "n = %f",  1.000501f), "n = 1.001");
    test_str_eq(fmt(&m, "n = %f", -1.000501f), "n = -1.001");
    test_str_eq(fmt(&m, "n = %f", 0.3f), "n = 0.300");

    test_str_eq(fmt(&m, "%s%s-%s", "AAAA", "BBBB", "C"), "AAAABBBB-C");

    test_str_eq(fmt(&m, "n=%4f", 1.234), "n=   1.234");
    test_str_eq(fmt(&m, "n=%1f", 1.234), "n=1.234");
    test_str_eq(fmt(&m, "n=%6i", 1234), "n=  1234");
    test_str_eq(fmt(&m, "n=%4i", 1234), "n=1234");
    test_str_eq(fmt(&m, "n=%1i", 1234), "n=1234");
    test_str_eq(fmt(&m, "n=%06x", 0x001234), "n=0x001234");
    test_str_eq(fmt(&m, "c=%c!", 'A'), "c=A!");

    test_str_eq(fmt(&m, "n=%+i",  1234), "n=+1234");
    test_str_eq(fmt(&m, "n=%+i", -1234), "n=-1234");
    return 0;
}

void main_update(void *handle) { os_exit(0); }
