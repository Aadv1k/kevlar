#include "../src/kevlar_markdown.h"
#include "../lib/unity/unity.h"
#include "../src/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void check_count_and_type(Md_Ast *ast, size_t c_count, NodeType typ) {
    TEST_ASSERT_EQUAL_INT(typ, ast->node_type);
    TEST_ASSERT_EQUAL_size_t(c_count, ast->c_count);
}

static void match_text_node_text(Md_Ast *txt_node, const char *dest) {
    TEST_ASSERT_EQUAL_INT(MD_TEXT_NODE, txt_node->node_type);
    TEST_ASSERT_EQUAL_STRING(dest, txt_node->opt.text_opt.data);
}

/* ══════════════════════════════════════════════════════════════
 * HEADING TESTS
 * ══════════════════════════════════════════════════════════════ */

static void test_md_heading_simple(void) {
    Md_Ast *ast = kevlar_md_generate_ast("# Foo\n");
    check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    match_text_node_text(ast->children[0]->children[0], "Foo");
    kevlar_md_free_ast(ast);
}

static void test_md_heading_no_space_is_para(void) {
    Md_Ast *ast = kevlar_md_generate_ast("###");
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    kevlar_md_free_ast(ast);
}

static void test_md_heading_level5(void) {
    Md_Ast *ast = kevlar_md_generate_ast("##### A");
    check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    match_text_node_text(ast->children[0]->children[0], "A");
    kevlar_md_free_ast(ast);
}

static void test_md_heading_too_many_hashes_is_para(void) {
    Md_Ast *ast = kevlar_md_generate_ast("########## Should NOT be a heading");
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    match_text_node_text(ast->children[0]->children[0], "########## Should NOT be a heading");
    kevlar_md_free_ast(ast);
}

static void test_md_heading_trailing_space_no_children(void) {
    Md_Ast *ast = kevlar_md_generate_ast("## ");
    check_count_and_type(ast->children[0], 0, MD_HEADING_NODE);
    kevlar_md_free_ast(ast);
}

static void test_md_heading_content_with_hashes(void) {
    Md_Ast *ast = kevlar_md_generate_ast("### ## ### # ## # # # # ## ##### ");
    check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    match_text_node_text(ast->children[0]->children[0], "## ### # ## # # # # ## ##### ");
    kevlar_md_free_ast(ast);
}

static void test_md_heading_tab_separator(void) {
    Md_Ast *ast = kevlar_md_generate_ast("#\t\t\t\tHello");
    check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    match_text_node_text(ast->children[0]->children[0], "Hello");
    kevlar_md_free_ast(ast);
}

static void test_md_heading_unicode(void) {
    Md_Ast *ast = kevlar_md_generate_ast("# 你好世界 🌍");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    TEST_ASSERT_EQUAL_INT(1, ast->children[0]->opt.h_opt.level);
    match_text_node_text(ast->children[0]->children[0], "你好世界 🌍");
    kevlar_md_free_ast(ast);
}

static void test_md_heading_levels_descending(void) {
    Md_Ast *ast = kevlar_md_generate_ast("# H1\n## H2\n### H3\n#### H4\n##### H5\n###### H6");
    check_count_and_type(ast, 6, MD_ROOT_NODE);
    for (int i = 0; i < 6; i++) {
        check_count_and_type(ast->children[i], 1, MD_HEADING_NODE);
        TEST_ASSERT_EQUAL_INT(i + 1, ast->children[i]->opt.h_opt.level);
    }
    kevlar_md_free_ast(ast);
}

/* ══════════════════════════════════════════════════════════════
 * INLINE CONTENT TESTS
 * ══════════════════════════════════════════════════════════════ */

static void test_md_content_em_only(void) {
    Md_Ast *ast = kevlar_md_generate_ast("*Bedazzled*");
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "Bedazzled");
    kevlar_md_free_ast(ast);
}

static void test_md_content_em_in_sentence(void) {
    Md_Ast *ast = kevlar_md_generate_ast("Hello, *World*!");
    check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    match_text_node_text(ast->children[0]->children[0], "Hello, ");
    check_count_and_type(ast->children[0]->children[1], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[1]->children[0], "World");
    check_count_and_type(ast->children[0]->children[2], 0, MD_TEXT_NODE);
    match_text_node_text(ast->children[0]->children[2], "!");
    kevlar_md_free_ast(ast);
}

static void test_md_content_em_unicode(void) {
    Md_Ast *ast = kevlar_md_generate_ast("*नमस्ते* दुनिया! 👋🌏");
    check_count_and_type(ast->children[0], 2, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "नमस्ते");
    check_count_and_type(ast->children[0]->children[1], 0, MD_TEXT_NODE);
    match_text_node_text(ast->children[0]->children[1], " दुनिया! 👋🌏");
    kevlar_md_free_ast(ast);
}

static void test_md_content_mixed_inline(void) {
    Md_Ast *ast = kevlar_md_generate_ast("The _quick_ *brown* **fox** ***jumps*** ~~over~~ the lazy dog");
    check_count_and_type(ast->children[0], 11, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "The ");
    check_count_and_type(ast->children[0]->children[1], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[1]->children[0], "quick");
    match_text_node_text(ast->children[0]->children[2], " ");
    check_count_and_type(ast->children[0]->children[3], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[3]->children[0], "brown");
    match_text_node_text(ast->children[0]->children[4], " ");
    check_count_and_type(ast->children[0]->children[5], 1, MD_STRONG_NODE);
    match_text_node_text(ast->children[0]->children[5]->children[0], "fox");
    match_text_node_text(ast->children[0]->children[2], " ");
    check_count_and_type(ast->children[0]->children[7], 1, MD_STRONG_EM_NODE);
    match_text_node_text(ast->children[0]->children[7]->children[0], "jumps");
    kevlar_md_free_ast(ast);
}

static void test_md_content_nested_em_in_strong(void) {
    Md_Ast *ast = kevlar_md_generate_ast("**Bold with *nested emphasis* inside**");
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 3, MD_STRONG_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "Bold with ");
    check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[1]->children[0], "nested emphasis");
    kevlar_md_free_ast(ast);
}

static void test_md_content_unclosed_em_is_text(void) {
    Md_Ast *ast = kevlar_md_generate_ast("This has *unclosed emphasis");
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    match_text_node_text(ast->children[0]->children[0], "This has *unclosed emphasis");
    kevlar_md_free_ast(ast);
}

static void test_md_content_strikethrough_only(void) {
    Md_Ast *ast = kevlar_md_generate_ast("~~deleted text~~");
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_DEL_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "deleted text");
    kevlar_md_free_ast(ast);
}

static void test_md_content_strikethrough_with_em(void) {
    Md_Ast *ast = kevlar_md_generate_ast("~~deleted *with emphasis*~~");
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 2, MD_DEL_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "deleted ");
    check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[1]->children[0], "with emphasis");
    kevlar_md_free_ast(ast);
}

static void test_md_content_empty_string(void) {
    Md_Ast *ast = kevlar_md_generate_ast("");
    check_count_and_type(ast, 0, MD_ROOT_NODE);
    kevlar_md_free_ast(ast);
}

static void test_md_content_plain_text(void) {
    Md_Ast *ast = kevlar_md_generate_ast("Just plain text with no formatting");
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "Just plain text with no formatting");
    kevlar_md_free_ast(ast);
}

static void test_md_content_special_chars(void) {
    Md_Ast *ast = kevlar_md_generate_ast("Text with <html>, &amp;, \"quotes\", and 'apostrophes'");
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0],
                         "Text with <html>, &amp;, \"quotes\", and 'apostrophes'");
    kevlar_md_free_ast(ast);
}

static void test_md_content_adjacent_em(void) {
    Md_Ast *ast = kevlar_md_generate_ast("*no space*immediately*adjacent*");
    check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "no space");
    match_text_node_text(ast->children[0]->children[1], "immediately");
    check_count_and_type(ast->children[0]->children[2], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[2]->children[0], "adjacent");
    kevlar_md_free_ast(ast);
}

static void test_md_content_underscore_vs_asterisk(void) {
    Md_Ast *ast = kevlar_md_generate_ast("_underscore_ and *asterisk* emphasis");
    check_count_and_type(ast->children[0], 4, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "underscore");
    check_count_and_type(ast->children[0]->children[2], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[2]->children[0], "asterisk");
    kevlar_md_free_ast(ast);
}

static void test_md_content_triple_em(void) {
    Md_Ast *ast = kevlar_md_generate_ast("***both bold and italic***");
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_STRONG_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "both bold and italic");
    kevlar_md_free_ast(ast);
}

static void test_md_content_triple_em_in_sentence(void) {
    Md_Ast *ast = kevlar_md_generate_ast("Start ***triple emphasis*** end");
    check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "Start ");
    check_count_and_type(ast->children[0]->children[1], 1, MD_STRONG_EM_NODE);
    match_text_node_text(ast->children[0]->children[1]->children[0], "triple emphasis");
    match_text_node_text(ast->children[0]->children[2], " end");
    kevlar_md_free_ast(ast);
}

static void test_md_content_unicode_stress(void) {
    Md_Ast *ast = kevlar_md_generate_ast("*日本語*, **中文**, ***العربية***, ~~עִברִית~~");
    check_count_and_type(ast->children[0], 7, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "日本語");
    check_count_and_type(ast->children[0]->children[2], 1, MD_STRONG_NODE);
    match_text_node_text(ast->children[0]->children[2]->children[0], "中文");
    check_count_and_type(ast->children[0]->children[4], 1, MD_STRONG_EM_NODE);
    match_text_node_text(ast->children[0]->children[4]->children[0], "العربية");
    check_count_and_type(ast->children[0]->children[6], 1, MD_DEL_NODE);
    match_text_node_text(ast->children[0]->children[6]->children[0], "עִברִית");
    kevlar_md_free_ast(ast);
}

static void test_md_content_emojis_with_formatting(void) {
    Md_Ast *ast = kevlar_md_generate_ast("*🔥fire🔥* **💪strong💪** ***🚀rocket🚀***");
    check_count_and_type(ast->children[0], 5, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "🔥fire🔥");
    check_count_and_type(ast->children[0]->children[2], 1, MD_STRONG_NODE);
    match_text_node_text(ast->children[0]->children[2]->children[0], "💪strong💪");
    check_count_and_type(ast->children[0]->children[4], 1, MD_STRONG_EM_NODE);
    match_text_node_text(ast->children[0]->children[4]->children[0], "🚀rocket🚀");
    kevlar_md_free_ast(ast);
}

static void test_md_content_escaped_asterisks(void) {
    Md_Ast *ast = kevlar_md_generate_ast("\\*not emphasis\\* but *this is*");
    check_count_and_type(ast->children[0], 2, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "*not emphasis* but ");
    check_count_and_type(ast->children[0]->children[1], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[1]->children[0], "this is");
    kevlar_md_free_ast(ast);
}

static void test_md_content_whitespace_only_para(void) {
    Md_Ast *ast = kevlar_md_generate_ast("     \t\t\t     ");
    check_count_and_type(ast->children[0], 0, MD_PARA_NODE);
    kevlar_md_free_ast(ast);
}

static void test_md_content_long_text_performance(void) {
    char long_text[10000];
    memset(long_text, 'a', 9999);
    long_text[9999] = '\0';
    Md_Ast *ast = kevlar_md_generate_ast(long_text);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    kevlar_md_free_ast(ast);
}

static void test_md_content_deep_nesting(void) {
    Md_Ast *ast = kevlar_md_generate_ast("**strong *em ~~del _nested_ del~~ em* strong**");
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 3, MD_STRONG_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "strong ");
    check_count_and_type(ast->children[0]->children[0]->children[1], 3, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[1]->children[0], "em ");
    check_count_and_type(ast->children[0]->children[0]->children[1]->children[1], 3, MD_DEL_NODE);
    kevlar_md_free_ast(ast);
}

static void test_md_content_single_char_em(void) {
    Md_Ast *ast = kevlar_md_generate_ast("*a* **b** ***c***");
    check_count_and_type(ast->children[0], 5, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "a");
    check_count_and_type(ast->children[0]->children[2], 1, MD_STRONG_NODE);
    match_text_node_text(ast->children[0]->children[2]->children[0], "b");
    check_count_and_type(ast->children[0]->children[4], 1, MD_STRONG_EM_NODE);
    match_text_node_text(ast->children[0]->children[4]->children[0], "c");
    kevlar_md_free_ast(ast);
}

static void test_md_content_em_at_start_and_end(void) {
    Md_Ast *ast = kevlar_md_generate_ast("*start* middle **end**");
    check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "start");
    match_text_node_text(ast->children[0]->children[1], " middle ");
    check_count_and_type(ast->children[0]->children[2], 1, MD_STRONG_NODE);
    match_text_node_text(ast->children[0]->children[2]->children[0], "end");
    kevlar_md_free_ast(ast);
}

static void test_md_content_multiple_spaces_between_formatted(void) {
    Md_Ast *ast = kevlar_md_generate_ast("*em*     **strong**");
    check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[1], "     ");
    check_count_and_type(ast->children[0]->children[2], 1, MD_STRONG_NODE);
    kevlar_md_free_ast(ast);
}

static void test_md_content_formatting_with_numbers(void) {
    Md_Ast *ast = kevlar_md_generate_ast("*123* **456** ***789***");
    check_count_and_type(ast->children[0], 5, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "123");
    check_count_and_type(ast->children[0]->children[2], 1, MD_STRONG_NODE);
    match_text_node_text(ast->children[0]->children[2]->children[0], "456");
    check_count_and_type(ast->children[0]->children[4], 1, MD_STRONG_EM_NODE);
    match_text_node_text(ast->children[0]->children[4]->children[0], "789");
    kevlar_md_free_ast(ast);
}

static void test_md_content_punctuation_heavy(void) {
    Md_Ast *ast = kevlar_md_generate_ast("*Hello!?* **What... the?** ***Really?!***");
    check_count_and_type(ast->children[0], 5, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "Hello!?");
    check_count_and_type(ast->children[0]->children[2], 1, MD_STRONG_NODE);
    match_text_node_text(ast->children[0]->children[2]->children[0], "What... the?");
    check_count_and_type(ast->children[0]->children[4], 1, MD_STRONG_EM_NODE);
    match_text_node_text(ast->children[0]->children[4]->children[0], "Really?!");
    kevlar_md_free_ast(ast);
}

static void test_md_content_complex_document(void) {
    Md_Ast *ast = kevlar_md_generate_ast(
        "# Document Title\n\n"
        "This is the *introduction* with **bold** text.\n\n"
        "## First Section\n\n"
        "Content with ~~strikethrough~~ and ***bold italic***.\n"
        "Second line of same paragraph.\n\n"
        "### Subsection A\n\n"
        "More content here.\n\n"
        "### Subsection B\n\n"
        "Final paragraph with *formatting*."
    );

    check_count_and_type(ast, 8, MD_ROOT_NODE);

    check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    TEST_ASSERT_EQUAL_INT(1, ast->children[0]->opt.h_opt.level);
    match_text_node_text(ast->children[0]->children[0], "Document Title");

    check_count_and_type(ast->children[1], 5, MD_PARA_NODE);
    match_text_node_text(ast->children[1]->children[0], "This is the ");
    check_count_and_type(ast->children[1]->children[1], 1, MD_EM_NODE);
    match_text_node_text(ast->children[1]->children[1]->children[0], "introduction");
    match_text_node_text(ast->children[1]->children[2], " with ");
    check_count_and_type(ast->children[1]->children[3], 1, MD_STRONG_NODE);
    match_text_node_text(ast->children[1]->children[3]->children[0], "bold");
    match_text_node_text(ast->children[1]->children[4], " text.");

    check_count_and_type(ast->children[2], 1, MD_HEADING_NODE);
    TEST_ASSERT_EQUAL_INT(2, ast->children[2]->opt.h_opt.level);
    match_text_node_text(ast->children[2]->children[0], "First Section");

    check_count_and_type(ast->children[3], 5, MD_PARA_NODE);
    match_text_node_text(ast->children[3]->children[0], "Content with ");
    check_count_and_type(ast->children[3]->children[1], 1, MD_DEL_NODE);
    match_text_node_text(ast->children[3]->children[1]->children[0], "strikethrough");
    match_text_node_text(ast->children[3]->children[2], " and ");
    check_count_and_type(ast->children[3]->children[3], 1, MD_STRONG_EM_NODE);
    match_text_node_text(ast->children[3]->children[3]->children[0], "bold italic");
    match_text_node_text(ast->children[3]->children[4], ".\nSecond line of same paragraph.");

    check_count_and_type(ast->children[4], 1, MD_HEADING_NODE);
    TEST_ASSERT_EQUAL_INT(3, ast->children[4]->opt.h_opt.level);
    match_text_node_text(ast->children[4]->children[0], "Subsection A");

    check_count_and_type(ast->children[5], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[5]->children[0], "More content here.");

    check_count_and_type(ast->children[6], 1, MD_HEADING_NODE);
    TEST_ASSERT_EQUAL_INT(3, ast->children[6]->opt.h_opt.level);
    match_text_node_text(ast->children[6]->children[0], "Subsection B");

    check_count_and_type(ast->children[7], 3, MD_PARA_NODE);
    match_text_node_text(ast->children[7]->children[0], "Final paragraph with ");
    check_count_and_type(ast->children[7]->children[1], 1, MD_EM_NODE);
    match_text_node_text(ast->children[7]->children[1]->children[0], "formatting");
    match_text_node_text(ast->children[7]->children[2], ".");

    kevlar_md_free_ast(ast);
}

/* ══════════════════════════════════════════════════════════════
 * INLINE CODE TESTS
 * ══════════════════════════════════════════════════════════════ */

static void test_md_inline_code_simple(void) {
    Md_Ast *ast = kevlar_md_generate_ast("`code`");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0]->children[0], "code");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_in_sentence(void) {
    Md_Ast *ast = kevlar_md_generate_ast("Use the `printf` function here.");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "Use the ");
    check_count_and_type(ast->children[0]->children[1], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[1]->children[0], "printf");
    match_text_node_text(ast->children[0]->children[2], " function here.");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_multiple(void) {
    Md_Ast *ast = kevlar_md_generate_ast("`foo` and `bar` and `baz`");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 5, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0]->children[0], "foo");
    match_text_node_text(ast->children[0]->children[1], " and ");
    check_count_and_type(ast->children[0]->children[2], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[2]->children[0], "bar");
    match_text_node_text(ast->children[0]->children[3], " and ");
    check_count_and_type(ast->children[0]->children[4], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[4]->children[0], "baz");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_no_formatting_inside(void) {
    Md_Ast *ast = kevlar_md_generate_ast("`*not* **emphasis**`");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0]->children[0], "*not* **emphasis**");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_no_strikethrough_inside(void) {
    Md_Ast *ast = kevlar_md_generate_ast("`~~not deleted~~`");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0]->children[0], "~~not deleted~~");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_escaped_backtick(void) {
    Md_Ast *ast = kevlar_md_generate_ast("This is \\`not code\\` text.");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "This is `not code` text.");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_escaped_asterisk(void) {
    Md_Ast *ast = kevlar_md_generate_ast("\\*not emphasis\\*");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "*not emphasis*");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_escaped_underscore(void) {
    Md_Ast *ast = kevlar_md_generate_ast("\\_not emphasis\\_");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "_not emphasis_");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_escaped_tilde(void) {
    Md_Ast *ast = kevlar_md_generate_ast("\\~\\~not deleted\\~\\~");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "~~not deleted~~");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_escaped_hash(void) {
    Md_Ast *ast = kevlar_md_generate_ast("\\# Not a heading");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "# Not a heading");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_escaped_backslash(void) {
    Md_Ast *ast = kevlar_md_generate_ast("Double backslash: \\\\");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "Double backslash: \\");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_backslash_before_nonspecial(void) {
    Md_Ast *ast = kevlar_md_generate_ast("\\a normal text");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "\\a normal text");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_with_spaces(void) {
    Md_Ast *ast = kevlar_md_generate_ast("` spaced out `");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0]->children[0], " spaced out ");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_empty_is_text(void) {
    Md_Ast *ast = kevlar_md_generate_ast("``");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "``");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_unclosed_is_text(void) {
    Md_Ast *ast = kevlar_md_generate_ast("`unclosed code");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "`unclosed code");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_in_heading(void) {
    Md_Ast *ast = kevlar_md_generate_ast("# Heading with `code` inside");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 3, MD_HEADING_NODE);
    TEST_ASSERT_EQUAL_INT(1, ast->children[0]->opt.h_opt.level);
    match_text_node_text(ast->children[0]->children[0], "Heading with ");
    check_count_and_type(ast->children[0]->children[1], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[1]->children[0], "code");
    match_text_node_text(ast->children[0]->children[2], " inside");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_inside_em(void) {
    Md_Ast *ast = kevlar_md_generate_ast("*emphasis `code` more emphasis*");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 3, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "emphasis ");
    check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0]->children[1]->children[0], "code");
    match_text_node_text(ast->children[0]->children[0]->children[2], " more emphasis");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_adjacent_blocks(void) {
    Md_Ast *ast = kevlar_md_generate_ast("`first``second`");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 2, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0]->children[0], "first");
    check_count_and_type(ast->children[0]->children[1], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[1]->children[0], "second");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_with_html_tags(void) {
    Md_Ast *ast = kevlar_md_generate_ast("`<div>HTML</div>`");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0]->children[0], "<div>HTML</div>");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_with_special_chars(void) {
    Md_Ast *ast = kevlar_md_generate_ast("`!@#$%^&*()_+-=[]{}|;:',.<>?/`");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0]->children[0], "!@#$%^&*()_+-=[]{}|;:',.<>?/");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_double_backtick_delimiter(void) {
    Md_Ast *ast = kevlar_md_generate_ast("``code with ` backtick``");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0]->children[0], "code with ` backtick");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_mixed_escaping(void) {
    Md_Ast *ast = kevlar_md_generate_ast("Text with \\*escaped\\* and *real* emphasis and `code`.");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 5, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "Text with *escaped* and ");
    check_count_and_type(ast->children[0]->children[1], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[1]->children[0], "real");
    match_text_node_text(ast->children[0]->children[2], " emphasis and ");
    check_count_and_type(ast->children[0]->children[3], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[3]->children[0], "code");
    match_text_node_text(ast->children[0]->children[4], ".");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_does_not_span_paragraphs(void) {
    Md_Ast *ast = kevlar_md_generate_ast("`code start\n\ncode end`");
    check_count_and_type(ast, 2, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "`code start");
    check_count_and_type(ast->children[1], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[1]->children[0], "code end`");
    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_complex_document(void) {
    Md_Ast *ast = kevlar_md_generate_ast(
        "# Code Examples\n\n"
        "Use `printf(\"Hello\")` to print.\n\n"
        "**Important:** The `main()` function is *required*.\n\n"
        "## Escaping\n\n"
        "Use \\` to show a literal backtick."
    );

    check_count_and_type(ast, 5, MD_ROOT_NODE);

    check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    TEST_ASSERT_EQUAL_INT(1, ast->children[0]->opt.h_opt.level);
    match_text_node_text(ast->children[0]->children[0], "Code Examples");

    check_count_and_type(ast->children[1], 3, MD_PARA_NODE);
    match_text_node_text(ast->children[1]->children[0], "Use ");
    check_count_and_type(ast->children[1]->children[1], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[1]->children[1]->children[0], "printf(\"Hello\")");
    match_text_node_text(ast->children[1]->children[2], " to print.");

    check_count_and_type(ast->children[2], 6, MD_PARA_NODE);
    check_count_and_type(ast->children[2]->children[0], 1, MD_STRONG_NODE);
    match_text_node_text(ast->children[2]->children[0]->children[0], "Important:");
    match_text_node_text(ast->children[2]->children[1], " The ");
    check_count_and_type(ast->children[2]->children[2], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[2]->children[2]->children[0], "main()");
    match_text_node_text(ast->children[2]->children[3], " function is ");
    check_count_and_type(ast->children[2]->children[4], 1, MD_EM_NODE);
    match_text_node_text(ast->children[2]->children[4]->children[0], "required");
    match_text_node_text(ast->children[2]->children[5], ".");

    kevlar_md_free_ast(ast);
}

static void test_md_inline_code_unicode(void) {
    Md_Ast *ast = kevlar_md_generate_ast("`日本語` and `🚀emoji🔥`");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0]->children[0], "日本語");
    match_text_node_text(ast->children[0]->children[1], " and ");
    check_count_and_type(ast->children[0]->children[2], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[2]->children[0], "🚀emoji🔥");
    kevlar_md_free_ast(ast);
}

/* ══════════════════════════════════════════════════════════════
 * LINK TESTS
 * ══════════════════════════════════════════════════════════════ */

static void test_md_link_simple(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[link text](https://example.com)");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    TEST_ASSERT_EQUAL_size_t(strlen("https://example.com"),
                             ast->children[0]->children[0]->opt.link_opt.href_len);
    TEST_ASSERT_EQUAL_STRING_LEN("https://example.com",
                                 ast->children[0]->children[0]->opt.link_opt.href_str,
                                 ast->children[0]->children[0]->opt.link_opt.href_len);
    match_text_node_text(ast->children[0]->children[0]->children[0], "link text");
    kevlar_md_free_ast(ast);
}

static void test_md_link_in_sentence(void) {
    Md_Ast *ast = kevlar_md_generate_ast("Check out [this site](https://example.com) for more info.");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "Check out ");
    check_count_and_type(ast->children[0]->children[1], 1, MD_LINK_NODE);
    TEST_ASSERT_EQUAL_size_t(strlen("https://example.com"),
                             ast->children[0]->children[1]->opt.link_opt.href_len);
    match_text_node_text(ast->children[0]->children[1]->children[0], "this site");
    match_text_node_text(ast->children[0]->children[2], " for more info.");
    kevlar_md_free_ast(ast);
}

static void test_md_link_multiple(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[first](url1) and [second](url2) links.");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 4, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    TEST_ASSERT_EQUAL_size_t(strlen("url1"),
                             ast->children[0]->children[0]->opt.link_opt.href_len);
    match_text_node_text(ast->children[0]->children[0]->children[0], "first");
    match_text_node_text(ast->children[0]->children[1], " and ");
    check_count_and_type(ast->children[0]->children[2], 1, MD_LINK_NODE);
    TEST_ASSERT_EQUAL_size_t(strlen("url2"),
                             ast->children[0]->children[2]->opt.link_opt.href_len);
    match_text_node_text(ast->children[0]->children[2]->children[0], "second");
    match_text_node_text(ast->children[0]->children[3], " links.");
    kevlar_md_free_ast(ast);
}

static void test_md_link_with_em_in_text(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[*emphasized* text](url)");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 2, MD_LINK_NODE);
    check_count_and_type(ast->children[0]->children[0]->children[0], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0]->children[0], "emphasized");
    match_text_node_text(ast->children[0]->children[0]->children[1], " text");
    kevlar_md_free_ast(ast);
}

static void test_md_link_with_strong_in_text(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[**bold** link](url)");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 2, MD_LINK_NODE);
    check_count_and_type(ast->children[0]->children[0]->children[0], 1, MD_STRONG_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0]->children[0], "bold");
    match_text_node_text(ast->children[0]->children[0]->children[1], " link");
    kevlar_md_free_ast(ast);
}

static void test_md_link_with_code_in_text(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[see `code` here](url)");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 3, MD_LINK_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "see ");
    check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0]->children[1]->children[0], "code");
    match_text_node_text(ast->children[0]->children[0]->children[2], " here");
    kevlar_md_free_ast(ast);
}

static void test_md_link_with_multiple_formatting(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[*em* **strong** `code`](url)");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 5, MD_LINK_NODE);
    check_count_and_type(ast->children[0]->children[0]->children[0], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0]->children[0], "em");
    match_text_node_text(ast->children[0]->children[0]->children[1], " ");
    check_count_and_type(ast->children[0]->children[0]->children[2], 1, MD_STRONG_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[2]->children[0], "strong");
    match_text_node_text(ast->children[0]->children[0]->children[3], " ");
    check_count_and_type(ast->children[0]->children[0]->children[4], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0]->children[4]->children[0], "code");
    kevlar_md_free_ast(ast);
}

static void test_md_link_em_around_link(void) {
    Md_Ast *ast = kevlar_md_generate_ast("*emphasis [link](url) text*");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 3, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "emphasis ");
    check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_LINK_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[1]->children[0], "link");
    match_text_node_text(ast->children[0]->children[0]->children[2], " text");
    kevlar_md_free_ast(ast);
}

static void test_md_link_with_spaces_in_url(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[text](https://example.com/path with spaces)");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    TEST_ASSERT_EQUAL_size_t(strlen("https://example.com/path with spaces"),
                             ast->children[0]->children[0]->opt.link_opt.href_len);
    match_text_node_text(ast->children[0]->children[0]->children[0], "text");
    kevlar_md_free_ast(ast);
}

static void test_md_link_with_query_params(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[text](https://example.com?foo=bar&baz=qux)");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    TEST_ASSERT_EQUAL_size_t(strlen("https://example.com?foo=bar&baz=qux"),
                             ast->children[0]->children[0]->opt.link_opt.href_len);
    kevlar_md_free_ast(ast);
}

static void test_md_link_with_fragment(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[jump](#section)");
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    TEST_ASSERT_EQUAL_STRING_LEN("#section",
                                 ast->children[0]->children[0]->opt.link_opt.href_str,
                                 ast->children[0]->children[0]->opt.link_opt.href_len);
    match_text_node_text(ast->children[0]->children[0]->children[0], "jump");
    kevlar_md_free_ast(ast);
}

static void test_md_link_relative_url(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[relative](../path/to/file.html)");
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    TEST_ASSERT_EQUAL_size_t(strlen("../path/to/file.html"),
                             ast->children[0]->children[0]->opt.link_opt.href_len);
    match_text_node_text(ast->children[0]->children[0]->children[0], "relative");
    kevlar_md_free_ast(ast);
}

static void test_md_link_mailto(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[email](mailto:user@example.com)");
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    TEST_ASSERT_EQUAL_size_t(strlen("mailto:user@example.com"),
                             ast->children[0]->children[0]->opt.link_opt.href_len);
    match_text_node_text(ast->children[0]->children[0]->children[0], "email");
    kevlar_md_free_ast(ast);
}

static void test_md_link_adjacent(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[first](url1)[second](url2)");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 2, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "first");
    check_count_and_type(ast->children[0]->children[1], 1, MD_LINK_NODE);
    match_text_node_text(ast->children[0]->children[1]->children[0], "second");
    kevlar_md_free_ast(ast);
}

static void test_md_link_unclosed_bracket_is_text(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[unclosed link(url)");
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "[unclosed link(url)");
    kevlar_md_free_ast(ast);
}

static void test_md_link_unclosed_paren_is_text(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[text](unclosed");
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "[text](unclosed");
    kevlar_md_free_ast(ast);
}

static void test_md_link_missing_parens_is_text(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[text]url");
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "[text]url");
    kevlar_md_free_ast(ast);
}

static void test_md_link_space_between_bracket_and_paren_is_text(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[text] (url)");
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "[text] (url)");
    kevlar_md_free_ast(ast);
}

static void test_md_link_escaped_bracket_in_text(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[text \\[with bracket\\]](url)");
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "text [with bracket]");
    kevlar_md_free_ast(ast);
}

static void test_md_link_escaped_paren_in_url(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[text](url\\(with\\)parens)");
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    TEST_ASSERT_EQUAL_STRING_LEN("url(with)parens",
                                 ast->children[0]->children[0]->opt.link_opt.href_str,
                                 ast->children[0]->children[0]->opt.link_opt.href_len);
    match_text_node_text(ast->children[0]->children[0]->children[0], "text");
    kevlar_md_free_ast(ast);
}

static void test_md_link_in_heading(void) {
    Md_Ast *ast = kevlar_md_generate_ast("# Heading with [link](url)");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 2, MD_HEADING_NODE);
    TEST_ASSERT_EQUAL_INT(1, ast->children[0]->opt.h_opt.level);
    match_text_node_text(ast->children[0]->children[0], "Heading with ");
    check_count_and_type(ast->children[0]->children[1], 1, MD_LINK_NODE);
    match_text_node_text(ast->children[0]->children[1]->children[0], "link");
    kevlar_md_free_ast(ast);
}

static void test_md_link_soft_break_in_text(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[link\ntext](url)");
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "link\ntext");
    kevlar_md_free_ast(ast);
}

static void test_md_link_cannot_span_paragraphs(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[text\n\nmore](url)");
    check_count_and_type(ast, 2, MD_ROOT_NODE);
    match_text_node_text(ast->children[0]->children[0], "[text");
    match_text_node_text(ast->children[1]->children[0], "more](url)");
    kevlar_md_free_ast(ast);
}

static void test_md_link_unicode_in_text(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[日本語 🌏](url)");
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "日本語 🌏");
    kevlar_md_free_ast(ast);
}

static void test_md_link_unicode_in_url(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[text](https://例え.jp/パス)");
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    TEST_ASSERT_EQUAL_size_t(strlen("https://例え.jp/パス"),
                             ast->children[0]->children[0]->opt.link_opt.href_len);
    kevlar_md_free_ast(ast);
}

static void test_md_link_long_url(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[link](https://example.com/very/long/path/with/many/segments/and/query?param1=value1&param2=value2&param3=value3#fragment)");
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    TEST_ASSERT_GREATER_THAN(50,
                             (int)ast->children[0]->children[0]->opt.link_opt.href_len);
    match_text_node_text(ast->children[0]->children[0]->children[0], "link");
    kevlar_md_free_ast(ast);
}

static void test_md_link_complex_document(void) {
    Md_Ast *ast = kevlar_md_generate_ast(
        "# Documentation\n\n"
        "See the [official docs](https://docs.example.com) for details.\n\n"
        "## Resources\n\n"
        "Check [*this* guide](url1) and [**that** tutorial](url2).\n\n"
        "Contact us at [support@example.com](mailto:support@example.com)."
    );

    check_count_and_type(ast, 5, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    match_text_node_text(ast->children[0]->children[0], "Documentation");

    check_count_and_type(ast->children[1], 3, MD_PARA_NODE);
    match_text_node_text(ast->children[1]->children[0], "See the ");
    check_count_and_type(ast->children[1]->children[1], 1, MD_LINK_NODE);
    match_text_node_text(ast->children[1]->children[1]->children[0], "official docs");
    match_text_node_text(ast->children[1]->children[2], " for details.");

    check_count_and_type(ast->children[2], 1, MD_HEADING_NODE);
    match_text_node_text(ast->children[2]->children[0], "Resources");

    check_count_and_type(ast->children[3], 5, MD_PARA_NODE);
    match_text_node_text(ast->children[3]->children[0], "Check ");
    check_count_and_type(ast->children[3]->children[1], 2, MD_LINK_NODE);
    check_count_and_type(ast->children[3]->children[1]->children[0], 1, MD_EM_NODE);
    match_text_node_text(ast->children[3]->children[1]->children[0]->children[0], "this");
    match_text_node_text(ast->children[3]->children[1]->children[1], " guide");
    match_text_node_text(ast->children[3]->children[2], " and ");
    check_count_and_type(ast->children[3]->children[3], 2, MD_LINK_NODE);
    check_count_and_type(ast->children[3]->children[3]->children[0], 1, MD_STRONG_NODE);
    match_text_node_text(ast->children[3]->children[3]->children[0]->children[0], "that");
    match_text_node_text(ast->children[3]->children[3]->children[1], " tutorial");

    kevlar_md_free_ast(ast);
}

static void test_md_link_after_code(void) {
    Md_Ast *ast = kevlar_md_generate_ast("`code` followed by [link](url)");
    check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0]->children[0], "code");
    match_text_node_text(ast->children[0]->children[1], " followed by ");
    check_count_and_type(ast->children[0]->children[2], 1, MD_LINK_NODE);
    match_text_node_text(ast->children[0]->children[2]->children[0], "link");
    kevlar_md_free_ast(ast);
}

static void test_md_link_before_code(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[link](url) followed by `code`");
    check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0], "link");
    match_text_node_text(ast->children[0]->children[1], " followed by ");
    check_count_and_type(ast->children[0]->children[2], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[2]->children[0], "code");
    kevlar_md_free_ast(ast);
}

static void test_md_link_with_strikethrough_in_text(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[~~deleted~~ text](url)");
    check_count_and_type(ast->children[0]->children[0], 2, MD_LINK_NODE);
    check_count_and_type(ast->children[0]->children[0]->children[0], 1, MD_DEL_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0]->children[0], "deleted");
    match_text_node_text(ast->children[0]->children[0]->children[1], " text");
    kevlar_md_free_ast(ast);
}

static void test_md_link_multiple_in_heading(void) {
    Md_Ast *ast = kevlar_md_generate_ast("## Section [one](url1) and [two](url2)");
    check_count_and_type(ast->children[0], 4, MD_HEADING_NODE);
    TEST_ASSERT_EQUAL_INT(2, ast->children[0]->opt.h_opt.level);
    match_text_node_text(ast->children[0]->children[0], "Section ");
    check_count_and_type(ast->children[0]->children[1], 1, MD_LINK_NODE);
    match_text_node_text(ast->children[0]->children[1]->children[0], "one");
    match_text_node_text(ast->children[0]->children[2], " and ");
    check_count_and_type(ast->children[0]->children[3], 1, MD_LINK_NODE);
    match_text_node_text(ast->children[0]->children[3]->children[0], "two");
    kevlar_md_free_ast(ast);
}

static void test_md_link_all_formatting_types_in_text(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[*em* **strong** ***both*** ~~del~~ `code`](url)");
    check_count_and_type(ast->children[0]->children[0], 9, MD_LINK_NODE);
    check_count_and_type(ast->children[0]->children[0]->children[0], 1, MD_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[0]->children[0], "em");
    match_text_node_text(ast->children[0]->children[0]->children[1], " ");
    check_count_and_type(ast->children[0]->children[0]->children[2], 1, MD_STRONG_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[2]->children[0], "strong");
    match_text_node_text(ast->children[0]->children[0]->children[3], " ");
    check_count_and_type(ast->children[0]->children[0]->children[4], 1, MD_STRONG_EM_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[4]->children[0], "both");
    match_text_node_text(ast->children[0]->children[0]->children[5], " ");
    check_count_and_type(ast->children[0]->children[0]->children[6], 1, MD_DEL_NODE);
    match_text_node_text(ast->children[0]->children[0]->children[6]->children[0], "del");
    match_text_node_text(ast->children[0]->children[0]->children[7], " ");
    check_count_and_type(ast->children[0]->children[0]->children[8], 1, MD_INLINE_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0]->children[8]->children[0], "code");
    kevlar_md_free_ast(ast);
}

static void test_md_link_url_with_percent_encoding(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[link](https://example.com/path?q=%20%21%40%23)");
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    TEST_ASSERT_EQUAL_size_t(strlen("https://example.com/path?q=%20%21%40%23"),
                             ast->children[0]->children[0]->opt.link_opt.href_len);
    kevlar_md_free_ast(ast);
}

static void test_md_link_data_url(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[img](data:image/png;base64,iVBORw0KGgo=)");
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    TEST_ASSERT_EQUAL_size_t(strlen("data:image/png;base64,iVBORw0KGgo="),
                             ast->children[0]->children[0]->opt.link_opt.href_len);
    kevlar_md_free_ast(ast);
}

static void test_md_link_ftp_url(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[file](ftp://ftp.example.com/file.txt)");
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    TEST_ASSERT_EQUAL_size_t(strlen("ftp://ftp.example.com/file.txt"),
                             ast->children[0]->children[0]->opt.link_opt.href_len);
    kevlar_md_free_ast(ast);
}

static void test_md_link_just_fragment_url(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[top](#)");
    check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    TEST_ASSERT_EQUAL_size_t(1, ast->children[0]->children[0]->opt.link_opt.href_len);
    TEST_ASSERT_EQUAL_STRING_LEN("#",
                                 ast->children[0]->children[0]->opt.link_opt.href_str, 1);
    match_text_node_text(ast->children[0]->children[0]->children[0], "top");
    kevlar_md_free_ast(ast);
}

static void test_md_link_whitespace_only_text(void) {
    Md_Ast *ast = kevlar_md_generate_ast("[   ](url)");
    check_count_and_type(ast->children[0]->children[0], 0, MD_LINK_NODE);
    TEST_ASSERT_EQUAL_size_t(strlen("url"),
                             ast->children[0]->children[0]->opt.link_opt.href_len);
    kevlar_md_free_ast(ast);
}

/* ══════════════════════════════════════════════════════════════
 * FENCED CODE BLOCK TESTS
 * ══════════════════════════════════════════════════════════════ */

static void test_md_code_block_simple(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```\ncode\n```");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    TEST_ASSERT_TRUE(ast->children[0]->opt.code_opt.lang_str == NULL ||
                     ast->children[0]->opt.code_opt.lang_str_len == 0);
    match_text_node_text(ast->children[0]->children[0], "code");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_with_language(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```python\nprint('hello')\n```");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    TEST_ASSERT_EQUAL_size_t(6, ast->children[0]->opt.code_opt.lang_str_len);
    TEST_ASSERT_EQUAL_STRING_LEN("python", ast->children[0]->opt.code_opt.lang_str, 6);
    match_text_node_text(ast->children[0]->children[0], "print('hello')");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_multiple_lines(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```\nline1\nline2\nline3\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0], "line1\nline2\nline3");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_preserves_indentation(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```\n    indented\n        more indented\n    back\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0],
                         "    indented\n        more indented\n    back");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_with_empty_lines(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```\nline1\n\nline3\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0], "line1\n\nline3");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_before_paragraph(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```\ncode\n```\n\nParagraph text.");
    check_count_and_type(ast, 2, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0], "code");
    check_count_and_type(ast->children[1], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[1]->children[0], "Paragraph text.");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_after_paragraph(void) {
    Md_Ast *ast = kevlar_md_generate_ast("Paragraph text.\n\n```\ncode\n```");
    check_count_and_type(ast, 2, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "Paragraph text.");
    check_count_and_type(ast->children[1], 1, MD_CODE_BLOCK);
    match_text_node_text(ast->children[1]->children[0], "code");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_no_markdown_parsed_inside(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```\n*not emphasis*\n**not strong**\n[not link](url)\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0],
                         "*not emphasis*\n**not strong**\n[not link](url)");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_backticks_inside(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```\ncode with ` backtick\nand ``` three\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0],
                         "code with ` backtick\nand ``` three");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_language_javascript(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```javascript\nconst x = 42;\nconsole.log(x);\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    TEST_ASSERT_EQUAL_size_t(10, ast->children[0]->opt.code_opt.lang_str_len);
    TEST_ASSERT_EQUAL_STRING_LEN("javascript", ast->children[0]->opt.code_opt.lang_str, 10);
    match_text_node_text(ast->children[0]->children[0], "const x = 42;\nconsole.log(x);");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_language_c(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```c\nint main() {\n    return 0;\n}\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    TEST_ASSERT_EQUAL_size_t(1, ast->children[0]->opt.code_opt.lang_str_len);
    TEST_ASSERT_EQUAL_STRING_LEN("c", ast->children[0]->opt.code_opt.lang_str, 1);
    match_text_node_text(ast->children[0]->children[0], "int main() {\n    return 0;\n}");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_unclosed_is_para(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```\nunclosed code");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_not_at_line_start_is_para(void) {
    Md_Ast *ast = kevlar_md_generate_ast("Text ```\ncode\n```");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[0]->children[0], "Text ```\ncode\n```");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_heading_inside_not_parsed(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```\n# Not a heading\n## Also not\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0], "# Not a heading\n## Also not");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_multiple_blocks(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```\nblock1\n```\n\n```\nblock2\n```");
    check_count_and_type(ast, 2, MD_ROOT_NODE);
    match_text_node_text(ast->children[0]->children[0], "block1");
    match_text_node_text(ast->children[1]->children[0], "block2");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_different_languages(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```python\ncode1\n```\n\n```java\ncode2\n```");
    check_count_and_type(ast, 2, MD_ROOT_NODE);
    TEST_ASSERT_EQUAL_STRING_LEN("python", ast->children[0]->opt.code_opt.lang_str, 6);
    match_text_node_text(ast->children[0]->children[0], "code1");
    TEST_ASSERT_EQUAL_STRING_LEN("java", ast->children[1]->opt.code_opt.lang_str, 4);
    match_text_node_text(ast->children[1]->children[0], "code2");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_between_headings(void) {
    Md_Ast *ast = kevlar_md_generate_ast("# Title\n\n```\ncode\n```\n\n## Section");
    check_count_and_type(ast, 3, MD_ROOT_NODE);
    TEST_ASSERT_EQUAL_INT(1, ast->children[0]->opt.h_opt.level);
    match_text_node_text(ast->children[1]->children[0], "code");
    TEST_ASSERT_EQUAL_INT(2, ast->children[2]->opt.h_opt.level);
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_html_content(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```html\n<div class=\"container\">\n  <p>Hello</p>\n</div>\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    TEST_ASSERT_EQUAL_STRING_LEN("html", ast->children[0]->opt.code_opt.lang_str, 4);
    match_text_node_text(ast->children[0]->children[0],
                         "<div class=\"container\">\n  <p>Hello</p>\n</div>");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_special_chars(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```\n!@#$%^&*()_+-=[]{}|;':\",./<>?\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0], "!@#$%^&*()_+-=[]{}|;':\",./<>?");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_unicode(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```\n日本語\n中文\nעברית\n🚀🔥\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0], "日本語\n中文\nעברית\n🚀🔥");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_tabs(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```\nline\twith\ttabs\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    match_text_node_text(ast->children[0]->children[0], "line\twith\ttabs");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_language_sql(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```sql\nSELECT * FROM users WHERE id = 1;\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    TEST_ASSERT_EQUAL_STRING_LEN("sql", ast->children[0]->opt.code_opt.lang_str, 3);
    match_text_node_text(ast->children[0]->children[0], "SELECT * FROM users WHERE id = 1;");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_language_bash(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```bash\n#!/bin/bash\necho \"Hello\"\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    TEST_ASSERT_EQUAL_STRING_LEN("bash", ast->children[0]->opt.code_opt.lang_str, 4);
    match_text_node_text(ast->children[0]->children[0], "#!/bin/bash\necho \"Hello\"");
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_trailing_newline(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```\ncode\n\n```");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_language_with_dash(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```c-sharp\nvar x = 10;\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    TEST_ASSERT_EQUAL_size_t(7, ast->children[0]->opt.code_opt.lang_str_len);
    TEST_ASSERT_EQUAL_STRING_LEN("c-sharp", ast->children[0]->opt.code_opt.lang_str, 7);
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_language_cpp(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```c++\nint x = 10;\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    TEST_ASSERT_EQUAL_size_t(3, ast->children[0]->opt.code_opt.lang_str_len);
    TEST_ASSERT_EQUAL_STRING_LEN("c++", ast->children[0]->opt.code_opt.lang_str, 3);
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_language_with_number(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```f90\nprogram test\nend program\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    TEST_ASSERT_EQUAL_STRING_LEN("f90", ast->children[0]->opt.code_opt.lang_str, 3);
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_language_uppercase(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```PYTHON\ncode\n```");
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    TEST_ASSERT_EQUAL_size_t(6, ast->children[0]->opt.code_opt.lang_str_len);
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_complex_document(void) {
    Md_Ast *ast = kevlar_md_generate_ast(
        "# Code Examples\n\n"
        "Here's some Python:\n\n"
        "```python\ndef hello():\n    print('world')\n```\n\n"
        "And some JavaScript:\n\n"
        "```javascript\nconst hello = () => console.log('world');\n```\n\n"
        "## Summary\n\n"
        "Both examples print *hello world*."
    );

    check_count_and_type(ast, 7, MD_ROOT_NODE);

    check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    TEST_ASSERT_EQUAL_INT(1, ast->children[0]->opt.h_opt.level);
    match_text_node_text(ast->children[0]->children[0], "Code Examples");

    check_count_and_type(ast->children[1], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[1]->children[0], "Here's some Python:");

    check_count_and_type(ast->children[2], 1, MD_CODE_BLOCK);
    TEST_ASSERT_EQUAL_STRING_LEN("python", ast->children[2]->opt.code_opt.lang_str, 6);
    match_text_node_text(ast->children[2]->children[0], "def hello():\n    print('world')");

    check_count_and_type(ast->children[3], 1, MD_PARA_NODE);
    match_text_node_text(ast->children[3]->children[0], "And some JavaScript:");

    check_count_and_type(ast->children[4], 1, MD_CODE_BLOCK);
    TEST_ASSERT_EQUAL_STRING_LEN("javascript", ast->children[4]->opt.code_opt.lang_str, 10);
    match_text_node_text(ast->children[4]->children[0],
                         "const hello = () => console.log('world');");

    check_count_and_type(ast->children[5], 1, MD_HEADING_NODE);

    kevlar_md_free_ast(ast);
}

static void test_md_code_block_windows_line_endings(void) {
    Md_Ast *ast = kevlar_md_generate_ast("```\r\nline1\r\nline2\r\n```");
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    kevlar_md_free_ast(ast);
}

static void test_md_code_block_very_long(void) {
    char long_code[5000];
    memset(long_code, 'x', 4990);
    long_code[4990] = '\0';
    char full_markdown[5100];
    snprintf(full_markdown, sizeof(full_markdown), "```\n%s\n```", long_code);
    Md_Ast *ast = kevlar_md_generate_ast(full_markdown);
    check_count_and_type(ast, 1, MD_ROOT_NODE);
    check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    TEST_ASSERT_EQUAL_size_t(4991, ast->children[0]->children[0]->opt.text_opt.len);
    kevlar_md_free_ast(ast);
}

/* ══════════════════════════════════════════════════════════════
 * RUNNER
 * ══════════════════════════════════════════════════════════════ */

void test_markdown(void) {
    /* Headings */
    RUN_TEST(test_md_heading_simple);
    RUN_TEST(test_md_heading_no_space_is_para);
    RUN_TEST(test_md_heading_level5);
    RUN_TEST(test_md_heading_too_many_hashes_is_para);
    RUN_TEST(test_md_heading_trailing_space_no_children);
    RUN_TEST(test_md_heading_content_with_hashes);
    RUN_TEST(test_md_heading_tab_separator);
    RUN_TEST(test_md_heading_unicode);
    RUN_TEST(test_md_heading_levels_descending);

    /* Inline content */
    RUN_TEST(test_md_content_em_only);
    RUN_TEST(test_md_content_em_in_sentence);
    RUN_TEST(test_md_content_em_unicode);
    RUN_TEST(test_md_content_mixed_inline);
    RUN_TEST(test_md_content_nested_em_in_strong);
    RUN_TEST(test_md_content_unclosed_em_is_text);
    RUN_TEST(test_md_content_strikethrough_only);
    RUN_TEST(test_md_content_strikethrough_with_em);
    RUN_TEST(test_md_content_empty_string);
    RUN_TEST(test_md_content_plain_text);
    RUN_TEST(test_md_content_special_chars);
    RUN_TEST(test_md_content_adjacent_em);
    RUN_TEST(test_md_content_underscore_vs_asterisk);
    RUN_TEST(test_md_content_triple_em);
    RUN_TEST(test_md_content_triple_em_in_sentence);
    RUN_TEST(test_md_content_unicode_stress);
    RUN_TEST(test_md_content_emojis_with_formatting);
    RUN_TEST(test_md_content_escaped_asterisks);
    RUN_TEST(test_md_content_whitespace_only_para);
    RUN_TEST(test_md_content_long_text_performance);
    RUN_TEST(test_md_content_deep_nesting);
    RUN_TEST(test_md_content_single_char_em);
    RUN_TEST(test_md_content_em_at_start_and_end);
    RUN_TEST(test_md_content_multiple_spaces_between_formatted);
    RUN_TEST(test_md_content_formatting_with_numbers);
    RUN_TEST(test_md_content_punctuation_heavy);
    RUN_TEST(test_md_content_complex_document);

    /* Inline code & escaping */
    RUN_TEST(test_md_inline_code_simple);
    RUN_TEST(test_md_inline_code_in_sentence);
    RUN_TEST(test_md_inline_code_multiple);
    RUN_TEST(test_md_inline_code_no_formatting_inside);
    RUN_TEST(test_md_inline_code_no_strikethrough_inside);
    RUN_TEST(test_md_inline_code_escaped_backtick);
    RUN_TEST(test_md_inline_code_escaped_asterisk);
    RUN_TEST(test_md_inline_code_escaped_underscore);
    RUN_TEST(test_md_inline_code_escaped_tilde);
    RUN_TEST(test_md_inline_code_escaped_hash);
    RUN_TEST(test_md_inline_code_escaped_backslash);
    RUN_TEST(test_md_inline_code_backslash_before_nonspecial);
    RUN_TEST(test_md_inline_code_with_spaces);
    RUN_TEST(test_md_inline_code_empty_is_text);
    RUN_TEST(test_md_inline_code_unclosed_is_text);
    RUN_TEST(test_md_inline_code_in_heading);
    RUN_TEST(test_md_inline_code_inside_em);
    RUN_TEST(test_md_inline_code_adjacent_blocks);
    RUN_TEST(test_md_inline_code_with_html_tags);
    RUN_TEST(test_md_inline_code_with_special_chars);
    RUN_TEST(test_md_inline_code_double_backtick_delimiter);
    RUN_TEST(test_md_inline_code_mixed_escaping);
    RUN_TEST(test_md_inline_code_does_not_span_paragraphs);
    RUN_TEST(test_md_inline_code_complex_document);
    RUN_TEST(test_md_inline_code_unicode);

    /* Links */
    RUN_TEST(test_md_link_simple);
    RUN_TEST(test_md_link_in_sentence);
    RUN_TEST(test_md_link_multiple);
    RUN_TEST(test_md_link_with_em_in_text);
    RUN_TEST(test_md_link_with_strong_in_text);
    RUN_TEST(test_md_link_with_code_in_text);
    RUN_TEST(test_md_link_with_multiple_formatting);
    RUN_TEST(test_md_link_em_around_link);
    RUN_TEST(test_md_link_with_spaces_in_url);
    RUN_TEST(test_md_link_with_query_params);
    RUN_TEST(test_md_link_with_fragment);
    RUN_TEST(test_md_link_relative_url);
    RUN_TEST(test_md_link_mailto);
    RUN_TEST(test_md_link_adjacent);
    RUN_TEST(test_md_link_unclosed_bracket_is_text);
    RUN_TEST(test_md_link_unclosed_paren_is_text);
    RUN_TEST(test_md_link_missing_parens_is_text);
    RUN_TEST(test_md_link_space_between_bracket_and_paren_is_text);
    RUN_TEST(test_md_link_escaped_bracket_in_text);
    RUN_TEST(test_md_link_escaped_paren_in_url);
    RUN_TEST(test_md_link_in_heading);
    RUN_TEST(test_md_link_soft_break_in_text);
    RUN_TEST(test_md_link_cannot_span_paragraphs);
    RUN_TEST(test_md_link_unicode_in_text);
    RUN_TEST(test_md_link_unicode_in_url);
    RUN_TEST(test_md_link_long_url);
    RUN_TEST(test_md_link_complex_document);
    RUN_TEST(test_md_link_after_code);
    RUN_TEST(test_md_link_before_code);
    RUN_TEST(test_md_link_with_strikethrough_in_text);
    RUN_TEST(test_md_link_multiple_in_heading);
    RUN_TEST(test_md_link_all_formatting_types_in_text);
    RUN_TEST(test_md_link_url_with_percent_encoding);
    RUN_TEST(test_md_link_data_url);
    RUN_TEST(test_md_link_ftp_url);
    RUN_TEST(test_md_link_just_fragment_url);
    RUN_TEST(test_md_link_whitespace_only_text);

    RUN_TEST(test_md_code_block_simple);
    RUN_TEST(test_md_code_block_with_language);
    RUN_TEST(test_md_code_block_multiple_lines);
    RUN_TEST(test_md_code_block_preserves_indentation);
    RUN_TEST(test_md_code_block_with_empty_lines);
    RUN_TEST(test_md_code_block_before_paragraph);
    RUN_TEST(test_md_code_block_after_paragraph);
    RUN_TEST(test_md_code_block_no_markdown_parsed_inside);
    RUN_TEST(test_md_code_block_backticks_inside);
    RUN_TEST(test_md_code_block_language_javascript);
    RUN_TEST(test_md_code_block_language_c);
    RUN_TEST(test_md_code_block_unclosed_is_para);
    RUN_TEST(test_md_code_block_not_at_line_start_is_para);
    RUN_TEST(test_md_code_block_heading_inside_not_parsed);
    RUN_TEST(test_md_code_block_multiple_blocks);
    RUN_TEST(test_md_code_block_different_languages);
    RUN_TEST(test_md_code_block_between_headings);
    RUN_TEST(test_md_code_block_html_content);
    RUN_TEST(test_md_code_block_special_chars);
    RUN_TEST(test_md_code_block_unicode);
    RUN_TEST(test_md_code_block_tabs);
    RUN_TEST(test_md_code_block_language_sql);
    RUN_TEST(test_md_code_block_language_bash);
    RUN_TEST(test_md_code_block_trailing_newline);
    RUN_TEST(test_md_code_block_language_with_dash);
    RUN_TEST(test_md_code_block_language_cpp);
    RUN_TEST(test_md_code_block_language_with_number);
    RUN_TEST(test_md_code_block_language_uppercase);
    RUN_TEST(test_md_code_block_complex_document);
    RUN_TEST(test_md_code_block_windows_line_endings);
    RUN_TEST(test_md_code_block_very_long);
}
