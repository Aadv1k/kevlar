#include "../lib/unity/unity.h"
#include "../src/kevlar_ini.h"
#include "../src/utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <libgen.h>

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
    TEST_ASSERT_EQUAL_STRING("qux hello world", kevlar_ini_table_get("baz"));
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

void test_kevlar_ini_parse_should_handleComments(void) {
    ini_parser_error error;

    kevlar_ini_table_init();
    TEST_ASSERT_EQUAL(0, kevlar_ini_parse("# this is a comment\nfoo=bar", &error));
    TEST_ASSERT_EQUAL_STRING(kevlar_ini_table_get("foo"), "bar");
    kevlar_ini_table_destroy();

    kevlar_ini_table_init();
    TEST_ASSERT_EQUAL(0, kevlar_ini_parse("; this is a comment\nfoo=bar", &error));
    TEST_ASSERT_EQUAL_STRING("bar", kevlar_ini_table_get("foo"));
    kevlar_ini_table_destroy();

    kevlar_ini_table_init();
    TEST_ASSERT_EQUAL(0, kevlar_ini_parse("foo=bar ; inline comment", &error));
    TEST_ASSERT_EQUAL_STRING("bar", kevlar_ini_table_get("foo"));
    kevlar_ini_table_destroy();

    kevlar_ini_table_init();
    TEST_ASSERT_EQUAL(0, kevlar_ini_parse("foo=bar # inline comment", &error));
    TEST_ASSERT_EQUAL_STRING("bar", kevlar_ini_table_get("foo"));
    kevlar_ini_table_destroy();
}

void test_kevlar_ini_parse_should_handleRealisticFile(void) {
    ini_parser_error error;
    kevlar_ini_table_init();
    char path[1024];
    strcpy(path, __FILE__);
    char *dir = dirname(path);
    char fp[1024] = "/__fixtures__/test_ini_file_1.ini";
    utl_prepend(fp, dir);
    FILE *f = fopen(fp, "r");
    TEST_ASSERT_NOT_NULL(f);
    fseek(f, 0, SEEK_END); 
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* file_buffer = malloc(file_size + 1);
    fread(file_buffer, 1, file_size, f);
    file_buffer[file_size] = '\0';
    int ec = kevlar_ini_parse(file_buffer, &error);
    TEST_ASSERT_EQUAL(0, ec);

    // [site]
    TEST_ASSERT_EQUAL_STRING("Aadvik's Wicked Blog", kevlar_ini_table_get("site.title"));
    TEST_ASSERT_EQUAL_STRING("https://aadvikpandey.com", kevlar_ini_table_get("site.base_url"));
    TEST_ASSERT_EQUAL_STRING("A personal blog about systems programming, career, random musings, and (sometimes) fitness", kevlar_ini_table_get("site.description"));

    // [build]
    TEST_ASSERT_EQUAL_STRING("./dist", kevlar_ini_table_get("build.output_dir"));
    TEST_ASSERT_EQUAL_STRING("./posts", kevlar_ini_table_get("build.posts_dir"));
    TEST_ASSERT_EQUAL_STRING("true", kevlar_ini_table_get("build.minify_html"));
    TEST_ASSERT_EQUAL_STRING("README.md .gitignore LICENSE", kevlar_ini_table_get("build.ignore"));

    // [author]
    TEST_ASSERT_EQUAL_STRING("Aadvik", kevlar_ini_table_get("author.name"));
    TEST_ASSERT_EQUAL_STRING("hi@example.com", kevlar_ini_table_get("author.email"));
    TEST_ASSERT_EQUAL_STRING("Self-taught engineer based in Bangalore. Interested in compilers and systems. Currently building Kevlar.", kevlar_ini_table_get("author.bio"));

    // [author.social]
    TEST_ASSERT_EQUAL_STRING("https://github.com/aadv1k", kevlar_ini_table_get("author.social.github"));
    TEST_ASSERT_EQUAL_STRING("none", kevlar_ini_table_get("author.social.twitter"));

    // [theme]
    TEST_ASSERT_EQUAL_STRING("default", kevlar_ini_table_get("theme.name"));
    TEST_ASSERT_EQUAL_STRING("10", kevlar_ini_table_get("theme.posts_per_page"));

    // [theme.default]
    TEST_ASSERT_EQUAL_STRING("true", kevlar_ini_table_get("theme.default.show_dates"));
    TEST_ASSERT_EQUAL_STRING("dark", kevlar_ini_table_get("theme.default.accent"));
    TEST_ASSERT_EQUAL_STRING("Built with Kevlar. Fast and simple.", kevlar_ini_table_get("theme.default.footer_text"));

    // [theme.default.colors]
    TEST_ASSERT_EQUAL_STRING("#1a1a1a", kevlar_ini_table_get("theme.default.colors.background"));
    TEST_ASSERT_EQUAL_STRING("#f0f0f0", kevlar_ini_table_get("theme.default.colors.foreground"));
    TEST_ASSERT_EQUAL_STRING("#ff6600", kevlar_ini_table_get("theme.default.colors.accent"));

    // [theme.default.colors.dark]
    TEST_ASSERT_EQUAL_STRING("#000000", kevlar_ini_table_get("theme.default.colors.dark.background"));
    TEST_ASSERT_EQUAL_STRING("#ffffff", kevlar_ini_table_get("theme.default.colors.dark.foreground"));

    // [edge] — this is where your parser earns its keep
    TEST_ASSERT_EQUAL_STRING("https://example.com/path/to/something?q=1&p=2", kevlar_ini_table_get("edge.url"));
    TEST_ASSERT_EQUAL_STRING("./some/deeply/nested/../normalized/path", kevlar_ini_table_get("edge.path"));
    TEST_ASSERT_EQUAL_STRING(".", kevlar_ini_table_get("edge.empty_ish"));
    TEST_ASSERT_EQUAL_STRING("42", kevlar_ini_table_get("edge.number"));
    TEST_ASSERT_EQUAL_STRING("3.14", kevlar_ini_table_get("edge.float_ish"));
    TEST_ASSERT_EQUAL_STRING("value with = sign inside", kevlar_ini_table_get("edge.colon_sep"));
    TEST_ASSERT_EQUAL_STRING("value with : colon inside", kevlar_ini_table_get("edge.eq_sep"));

    free(file_buffer);
    kevlar_ini_table_destroy();
}


void test_ini(void) {
#if 0 
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
    RUN_TEST(test_kevlar_ini_parse_should_handleComments);
#endif
    RUN_TEST(test_kevlar_ini_parse_should_handleRealisticFile);
    
}
