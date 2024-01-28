#include "fmt.h"
#include "os_generic.h"


static bool test_str_eq_imp(char *comp, char *test) {
    os_print(comp);
    os_print("\n");
    os_print(test);
    os_print("\n");
    os_print("\n");
    return str_eq(comp, test);
}

static void test_str_eq(char *comp, char *test) {
    os_print("EQ\n");
    assert(test_str_eq_imp(comp, test));
}

static void test_str_not_eq(char *comp, char *test) {
    os_print("NOT EQ\n");
    assert(!test_str_eq_imp(comp, test));
}

void *main_init(int argc, char **argv) {
    mem m = {};
    test_str_eq(fmt(&m, "Hello World"), "Hello World");
    test_str_eq(fmt(&m, "n = %u",  1337), "n = 1337");
    test_str_eq(fmt(&m, "n = %i",  1337), "n = 1337");
    test_str_not_eq(fmt(&m, "n = %u", -1337), "n = -1337");
    test_str_eq(fmt(&m, "n = %i", -1337), "n = -1337");
    test_str_eq(fmt(&m, "n = %f", 1337.0f), "n = 1337.000");
    return 0;
}

void main_update(void *handle) { os_exit(0); }
