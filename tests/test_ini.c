#include "../lib/unity/unity.h"
#include "../src/kevlar_ini.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void test_function_should_hashCorrectly(void) {
    TEST_ASSERT_EQUAL_HEX(0xCC857C54, fnv1_hash("Hello, World!"));
}

void test_kevlarIniTableInit_should_initSuccessfully(void) {
    TEST_ASSERT_EQUAL(0, kevlar_ini_table_init());
    kevlar_ini_table_destroy();
}

void test__h_table_set_str_should_setNewValCorrectly(void) {
    ini_table* table = _h_table_init();
    TEST_ASSERT_NOT_NULL(table);

#define CHECK_KEY_VAL(k, v)                                          \
    do {                                                             \
        bool found = false;                                          \
        for (size_t i = 0; i < table->buckets; ++i) {               \
            if (table->nodes[i] == NULL) continue;                   \
            if (strcmp(table->nodes[i]->key, (k)) == 0 &&           \
                strcmp(table->nodes[i]->val, (v)) == 0) {           \
                found = true;                                        \
                break;                                               \
            }                                                        \
        }                                                            \
        TEST_ASSERT_TRUE(found);                                     \
    } while (0)

    _h_table_set_str(table, "foo", "bar");
    TEST_ASSERT_EQUAL(1, table->nodes_count);
    bool found = false;
    for (size_t i = 0; i <= table->buckets; ++i) {
        if (table->nodes[i] == NULL) continue;
        if (strcmp(table->nodes[i]->key, "foo") == 0 &&
            strcmp(table->nodes[i]->val, "bar") == 0) {
            found = true;
            break;
        }
    }
    TEST_ASSERT_TRUE(found);

    _h_table_set_str(table, "foo", "bazaar");
    TEST_ASSERT_EQUAL(1, table->nodes_count);
    CHECK_KEY_VAL("foo", "bazaar");

    _h_table_set_str(table, "foo", "ballistic");
    CHECK_KEY_VAL("foo", "ballistic");

    _h_table_set_str(table, "section.key", "val1");
    TEST_ASSERT_EQUAL(2, table->nodes_count);
    CHECK_KEY_VAL("section.key", "val1");

    _h_table_set_str(table, "section.key", "val2");
    TEST_ASSERT_EQUAL(2, table->nodes_count);
    CHECK_KEY_VAL("section.key", "val2");

    _h_table_set_str(table, "123", "numval");
    TEST_ASSERT_EQUAL(3, table->nodes_count);
    CHECK_KEY_VAL("123", "numval");

    _h_table_set_str(table, "123", "replaced");
    TEST_ASSERT_EQUAL(3, table->nodes_count);
    CHECK_KEY_VAL("123", "replaced");

#undef CHECK_KEY_VAL

    _h_table_destroy(table);
}

void test__h_table_get_should_returnCorrectly(void) {
    ini_table* table = _h_table_init();
    TEST_ASSERT_NOT_NULL(table);

#define CHECK_GET(k, v) \
    TEST_ASSERT_EQUAL_STRING((v), _h_table_get(table, (k)))

    TEST_ASSERT_NULL(_h_table_get(table, "foo"));

    _h_table_set_str(table, "foo", "bar");
    CHECK_GET("foo", "bar");

    _h_table_set_str(table, "foo", "baz");
    CHECK_GET("foo", "baz");

    TEST_ASSERT_NULL(_h_table_get(table, "missing"));

    _h_table_set_str(table, "a.b.c", "val");
    CHECK_GET("a.b.c", "val");
    TEST_ASSERT_NULL(_h_table_get(table, "a.b"));

    _h_table_set_str(table, "42", "num");
    CHECK_GET("42", "num");

#undef CHECK_GET

    _h_table_destroy(table);
}

void test_kevlar_ini_parse_should_parseSimpleKeyValCorrectly(void) {
    ini_parser_error error;

    kevlar_ini_table_init();
    TEST_ASSERT_EQUAL(0, kevlar_ini_parse("foo=bar", &error));
    TEST_ASSERT_EQUAL_STRING("bar", kevlar_ini_table_get("foo"));
    kevlar_ini_table_destroy();

    kevlar_ini_table_init();
    TEST_ASSERT_EQUAL(0, kevlar_ini_parse("foo:bar\nbaz:qux", &error));
    TEST_ASSERT_EQUAL_STRING("bar", kevlar_ini_table_get("foo"));
    TEST_ASSERT_EQUAL_STRING("qux", kevlar_ini_table_get("baz"));
    kevlar_ini_table_destroy();

    kevlar_ini_table_init();
    TEST_ASSERT_EQUAL(0, kevlar_ini_parse("baz:qux\n\thello\n\tworld", &error));
    TEST_ASSERT_NOT_NULL(kevlar_ini_table_get("baz"));
    // TODO: at some point, filter out the \t in multi-line strs
    TEST_ASSERT_EQUAL_STRING("qux\n\thello\n\tworld", kevlar_ini_table_get("baz"));
    kevlar_ini_table_destroy();
}

void test_kevlar_ini_parse_should_parseSectionLabels(void) {
    ini_parser_error error;

    kevlar_ini_table_init();
    TEST_ASSERT_EQUAL(0, kevlar_ini_parse("[foo]\nbar=baz", &error));
    TEST_ASSERT_EQUAL_STRING("baz", kevlar_ini_table_get("foo.bar"));
    kevlar_ini_table_destroy();
}

void test_kevlar_ini_parse_should_parseWhitespaceInKeyAndVal(void) {
    ini_parser_error error;

    kevlar_ini_table_init();
    TEST_ASSERT_EQUAL(0, kevlar_ini_parse("       foo     =      baz     ", &error));
    TEST_ASSERT_EQUAL_STRING("baz", kevlar_ini_table_get("foo"));
    kevlar_ini_table_destroy();
}

void test_kevlar_ini_parse_should_failOnUnexpectedClosingBracket(void) {
    ini_parser_error error;

    kevlar_ini_table_init();
    TEST_ASSERT_EQUAL(-1, kevlar_ini_parse("]foo", &error));
    TEST_ASSERT_EQUAL(INI_ERR_INVALID_SYNTAX, error.code);
    kevlar_ini_table_destroy();
}

void test_kevlar_ini_parse_should_failOnEmptySectionLabel(void) {
    ini_parser_error error;

    kevlar_ini_table_init();
    TEST_ASSERT_EQUAL(-1, kevlar_ini_parse("[]", &error));
    TEST_ASSERT_EQUAL(INI_ERR_INVALID_LABEL, error.code);
    kevlar_ini_table_destroy();
}

void test_kevlar_ini_parse_should_failOnWhitespaceOnlySectionLabel(void) {
    ini_parser_error error;

    kevlar_ini_table_init();
    TEST_ASSERT_EQUAL(-1, kevlar_ini_parse("[   ]", &error));
    TEST_ASSERT_EQUAL(INI_ERR_INVALID_LABEL, error.code);
    kevlar_ini_table_destroy();
}

void test_kevlar_ini_parse_should_failOnMissingClosingBracket(void) {
    ini_parser_error error;

    kevlar_ini_table_init();
    TEST_ASSERT_EQUAL(-1, kevlar_ini_parse("[foo", &error));
    TEST_ASSERT_EQUAL(INI_ERR_INVALID_LABEL, error.code);
    kevlar_ini_table_destroy();
}

void test_kevlar_ini_parse_should_failOnMissingKey(void) {
    ini_parser_error error;

    kevlar_ini_table_init();
    TEST_ASSERT_EQUAL(-1, kevlar_ini_parse("=bar", &error));
    TEST_ASSERT_EQUAL(INI_ERR_INVALID_KEY, error.code);
    kevlar_ini_table_destroy();
}

void test_kevlar_ini_parse_should_failOnEmptyValue(void) {
    ini_parser_error error;

    kevlar_ini_table_init();
    TEST_ASSERT_EQUAL(-1, kevlar_ini_parse("foo=", &error));
    TEST_ASSERT_EQUAL(INI_ERR_INVALID_SYNTAX, error.code);
    kevlar_ini_table_destroy();
}

void test_kevlar_ini_parse_should_reportCorrectLineOnError(void) {
    ini_parser_error error;

    kevlar_ini_table_init();
    TEST_ASSERT_EQUAL(-1, kevlar_ini_parse("foo=bar\n=baz", &error));
    TEST_ASSERT_EQUAL(INI_ERR_INVALID_KEY, error.code);
    TEST_ASSERT_EQUAL(2, error.line);
    kevlar_ini_table_destroy();
}

void test_ini(void) {
    RUN_TEST(test_function_should_hashCorrectly);
    RUN_TEST(test_kevlarIniTableInit_should_initSuccessfully);
    RUN_TEST(test__h_table_set_str_should_setNewValCorrectly);
    RUN_TEST(test__h_table_get_should_returnCorrectly);
    RUN_TEST(test_kevlar_ini_parse_should_parseSimpleKeyValCorrectly);
    RUN_TEST(test_kevlar_ini_parse_should_parseWhitespaceInKeyAndVal);
    RUN_TEST(test_kevlar_ini_parse_should_parseSectionLabels);
    RUN_TEST(test_kevlar_ini_parse_should_failOnUnexpectedClosingBracket);
    RUN_TEST(test_kevlar_ini_parse_should_failOnEmptySectionLabel);
    RUN_TEST(test_kevlar_ini_parse_should_failOnWhitespaceOnlySectionLabel);
    RUN_TEST(test_kevlar_ini_parse_should_failOnMissingClosingBracket);
    RUN_TEST(test_kevlar_ini_parse_should_failOnMissingKey);
    RUN_TEST(test_kevlar_ini_parse_should_failOnEmptyValue);
    RUN_TEST(test_kevlar_ini_parse_should_reportCorrectLineOnError);
}
