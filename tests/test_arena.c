#include "../lib/unity/unity.h"
#include "../src/kevlar_markdown_v2.h"

#include <string.h>

void test_arena_allocate_should_workCorrectly(void) {
    mem_arena *a = arena_create(KiB(1));

    const char* strA = "foo";
    char* ptrA = arena_allocate(a, strlen(strA));
    strcpy(ptrA, strA);

    TEST_ASSERT_NOT_NULL(ptrA);
    TEST_ASSERT_EQUAL_STRING(strA, ptrA);

    const char* strB = "bar";
    char* ptrB = arena_allocate(a, strlen(strB));
    strcpy(ptrB, strB);

    TEST_ASSERT_NOT_NULL(ptrB);
    TEST_ASSERT_EQUAL_STRING(strA, ptrA);
    TEST_ASSERT_EQUAL_STRING(strB, ptrB);

    arena_destroy(a);
}

void test_arena_allocate_should_handleOverflowGracefully(void) {
    mem_arena *a = arena_create(8);

    const char* strA = "foo";
    char* ptrA = arena_allocate(a, strlen(strA));
    strcpy(ptrA, strA);

    TEST_ASSERT_NOT_NULL(ptrA);
    TEST_ASSERT_EQUAL_STRING(strA, ptrA);

    const char* strB = "bar";
    char* ptrB = arena_allocate(a, strlen(strB));

    TEST_ASSERT_NULL(ptrB);
}


void test_arena(void) {
    RUN_TEST(test_arena_allocate_should_workCorrectly);
    RUN_TEST(test_arena_allocate_should_handleOverflowGracefully);
}
