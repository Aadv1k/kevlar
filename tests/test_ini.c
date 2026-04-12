#include "../lib/unity/unity.h"
#include "../src/kevlar_ini.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void test_function_should_hashCorrectly(void) {
    TEST_ASSERT_EQUAL_HEX(0xCC857C54, fnv1_hash("Hello, World!"));
}

void test_kevlarIniTableInit_should_initSuccessfully(void) {
    TEST_ASSERT_EQUAL(kevlar_ini_table_init(""), 0);
    kevlar_ini_table_destroy();
}

void test__h_table_set_str_should_setNewValCorrectly(void) {
    ini_table* table = _h_table_init();
    TEST_ASSERT_NOT_NULL(table);

    _h_table_set_str(table, "foo", "bar");
    TEST_ASSERT_EQUAL(table->nodes_count, 1);

    bool found = false;
    char* val = "bar";

    // The poor man's inline functions

    goto check_if_key_in_table;

    TEST_ASSERT_TRUE(found);

    found = false;
    val = "bazaar";
    _h_table_set_str(table, "foo", "bazaar");

    goto check_if_key_in_table;

    TEST_ASSERT_TRUE(found);

    found = false;
    val = "bazaar";
    _h_table_set_str(table, "foo", "ballistic");

    goto check_if_key_in_table;

    TEST_ASSERT_TRUE(found);

    _h_table_destroy(table);

check_if_key_in_table:
    for (size_t i = 0; i < table->buckets; ++i) {
        if (table->nodes[i] == NULL) continue;
        if (strcmp(table->nodes[i]->key, "foo") == 0) {
            if (strcmp(table->nodes[i]->as.val, val) == 0) found = true;
        }
    }
}

void test_ini(void) {
    RUN_TEST(test_function_should_hashCorrectly);
    RUN_TEST(test_kevlarIniTableInit_should_initSuccessfully);
    RUN_TEST(test__h_table_set_str_should_setNewValCorrectly);
}
