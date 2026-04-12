#include "../lib/unity/unity.h"
#include "../src/kevlar_ini.h"
#include <stdio.h>

void test_function_should_hashCorrectly(void) {
    TEST_ASSERT_EQUAL_HEX(0xCC857C54, fnv1_hash("Hello, World!"));
}

void test_kevlarIniTableInit_should_initSuccessfully(void) {
    TEST_ASSERT_EQUAL(kevlar_ini_table_init(""), 0);
    kevlar_ini_table_destroy();
}

void test_function_should_parseSimplestConfig(void) {
    TEST_IGNORE();
}

void test_ini(void) {
    RUN_TEST(test_function_should_hashCorrectly);
    RUN_TEST(test_kevlarIniTableInit_should_initSuccessfully);
    RUN_TEST(test_function_should_parseSimplestConfig);
}
