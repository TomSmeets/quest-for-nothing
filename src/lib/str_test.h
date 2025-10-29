#pragma once
#include "lib/str.h"
#include "lib/test.h"

static void str_test(Test *test) {
    Memory *mem = test->mem;

    // `S0` and `S()`
    TEST(str_eq(S0, S0) == 1);
    TEST(str_eq(S(""), S0) == 1);
    TEST(str_eq(S(""), S("")) == 1);
    TEST(str_eq(S("A"), S0) == 0);
    TEST(str_eq(S0, S("A")) == 0);
    TEST(str_eq(S("A"), S("A")) == 1);
    TEST(str_eq(S("Hello"), S("World")) == 0);
    TEST(str_eq(S("Hello"), S("Hello")) == 1);

    // str_eq()
    String s_hello = S("Hello ");
    String s_world = S("World!");
    TEST(str_eq(s_hello, s_hello) == true);
    TEST(str_eq(s_world, s_world) == true);
    TEST(str_eq(s_hello, s_world) == false);
    TEST(str_eq(s_world, s_hello) == false);

    // str_slice()
    TEST(str_eq(str_slice(S("ABCDEF"), 0, 6), S("ABCDEF")));
    TEST(str_eq(str_slice(S("ABCDEF"), 0, 3), S("ABC")));
    TEST(str_eq(str_slice(S("ABCDEF"), 3, 3), S("DEF")));
    TEST(str_eq(str_slice(S("ABCDEF"), 2, 1), S("C")));

    // str_starts_with()
    TEST(str_ends_with(S("ABCDEF"), S("F")) == 1);
    TEST(str_ends_with(S("ABCDEF"), S("DEF")) == 1);
    TEST(str_ends_with(S("ABCDEF"), S("ABCDEF")) == 1);
    TEST(str_ends_with(S("ABCDEF"), S("XABCDEF")) == 0);
    TEST(str_ends_with(S("ABCDEF"), S("ABCDEFX")) == 0);
    TEST(str_ends_with(S("ABCDEF"), S("X")) == 0);
    TEST(str_ends_with(S("ABCDEF"), S("")) == 1);
    TEST(str_ends_with(S(""), S("")) == 1);
    TEST(str_ends_with(S(""), S("A")) == 0);

    // str_ends_with()
    TEST(str_starts_with(S("ABCDEF"), S("A")) == 1);
    TEST(str_starts_with(S("ABCDEF"), S("ABC")) == 1);
    TEST(str_starts_with(S("ABCDEF"), S("ABCDEF")) == 1);
    TEST(str_starts_with(S("ABCDEF"), S("ABCDEFX")) == 0);
    TEST(str_starts_with(S("ABCDEF"), S("XABCDEF")) == 0);
    TEST(str_starts_with(S("ABCDEF"), S("X")) == 0);
    TEST(str_starts_with(S("ABCDEF"), S("")) == 1);
    TEST(str_starts_with(S(""), S("")) == 1);
    TEST(str_starts_with(S(""), S("A")) == 0);

    // str_take() / str_drop()
    TEST(str_eq(str_take_start(S("Hello World!"), 5), S("Hello")));
    TEST(str_eq(str_take_end(S("Hello World!"), 6), S("World!")));
    TEST(str_eq(str_drop_start(S("Hello World!"), 6), S("World!")));
    TEST(str_eq(str_drop_end(S("Hello World!"), 7), S("Hello")));

    // str_append()
    String s_greeting = str_append(mem, s_hello, s_world);
    TEST(str_eq(s_hello, S("Hello ")));
    TEST(str_eq(s_world, S("World!")));
    TEST(str_eq(s_greeting, S("Hello World!")));

    // str_replace()
    TEST(str_eq(str_replace(mem, S("Hello test World!"), 6, 4, S("AWESOME")), S("Hello AWESOME World!")));
    TEST(str_eq(str_replace(mem, S("Hello test World!"), 6, 5, S0), S("Hello World!")));
    TEST(str_eq(str_replace(mem, S("Hello test World!"), 0, 6, S0), S("test World!")));
    TEST(str_eq(str_replace(mem, S("Hello test World!"), 0, 17, S0), S0));
    TEST(str_eq(str_replace(mem, S0, 0, 0, S0), S0));

    // str_clone()
    String s_hello2 = str_clone(mem, s_hello);
    TEST(str_eq(s_hello2, s_hello));
    TEST(s_hello2.data != s_hello.data);
}
