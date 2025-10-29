#pragma once
#include "lib/chunk.h"
#include "lib/test.h"

static void chunk_test(Test *test) {
    Chunk *chunk_a = chunk_alloc();
    Chunk *chunk_b = chunk_alloc();

    TEST(chunk_a != 0);
    TEST(chunk_b != 0);
    TEST(chunk_a != chunk_b);

    ((u8 *)chunk_a)[CHUNK_HEADER_SIZE] = 'A';
    ((u8 *)chunk_a)[CHUNK_SIZE - 1] = 'Z';
    TEST(((u8 *)chunk_a)[CHUNK_HEADER_SIZE] == 'A');
    TEST(((u8 *)chunk_a)[CHUNK_SIZE - 1] == 'Z');

    // Free will always return to the cache
    chunk_free(chunk_a);
    chunk_free(chunk_b);

    Chunk *chunk_c = chunk_alloc();
    Chunk *chunk_d = chunk_alloc();
    TEST(chunk_c == chunk_b);
    TEST(chunk_d == chunk_a);
}
