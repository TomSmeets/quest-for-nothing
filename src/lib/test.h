#pragma once
#include "lib/fmt.h"

TYPEDEF_STRUCT(Test);
TYPEDEF_STRUCT(Test_Step);

struct Test_Step {
    char *file;
    u32 line;
    char *condition;
    bool result;
    Test_Step *next;
};

struct Test {
    Fmt *fmt;
    Memory *mem;
    char *last_file;
    Test_Step *step_first;
    Test_Step *step_last;
};

static Test *test_begin(void) {
    Memory *mem = G->tmp;
    Test *test = mem_struct(mem, Test);
    test->mem = mem;
    test->fmt = G->fmt;
    return test;
}

static void test_step_fmt(Test_Step *step, Fmt *fmt) {
    if (step->result)
        fmt_s(G->fmt, "[SUCC] ");
    else
        fmt_s(G->fmt, "[FAIL] ");
    fmt_s(G->fmt, step->file);
    fmt_s(G->fmt, " ");
    fmt_s(G->fmt, step->condition);
    fmt_s(G->fmt, "\n");
}

static void test_end(Test *test) {
    u32 succ_count = 0;
    u32 fail_count = 0;
    for (Test_Step *step = test->step_first; step; step = step->next) {
        if (step->result) {
            succ_count++;
        } else {
            fail_count++;
        }
    }

    fmt_s(test->fmt, "\n");
    fmt_s(test->fmt, "\n");
    bool failed = fail_count > 0;
    if (failed) {
        fmt_s(test->fmt, "Test Failed!\n");
    } else {
        fmt_s(test->fmt, "Test Success!\n");
    }
    for (Test_Step *step = test->step_first; step; step = step->next) {
        if (step->result) continue;
        fmt_s(G->fmt, step->file);
        fmt_s(G->fmt, ":");
        fmt_u(G->fmt, step->line);
        fmt_s(G->fmt, ": Test '");
        fmt_s(G->fmt, step->condition);
        fmt_s(G->fmt, "' Failed!");
        fmt_s(G->fmt, "\n");
    }
    os_exit(failed ? 1 : 0);
}

static void test_assert(Test *test, char *file, u32 line, char *condition, bool result) {
    Test_Step *step = mem_struct(test->mem, Test_Step);
    step->file = file;
    step->line = line;
    step->condition = condition;
    step->result = result;
    LIST_APPEND(test->step_first, test->step_last, step);

    if (file != test->last_file) {
        test->last_file = file;
        fmt_ss(test->fmt, "\n==== ", file, " ====\n");
    }

    if (step->result)
        fmt_s(G->fmt, "[SUCC] ");
    else
        fmt_s(G->fmt, "[FAIL] ");
    fmt_s(G->fmt, step->condition);
    fmt_s(G->fmt, "\n");
}

#define TEST(cond) test_assert(test, __FILE__, __LINE__, #cond, cond)
