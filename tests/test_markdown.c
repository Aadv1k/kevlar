#include "../src/kevlar_markdown.h"
#include "../src/utils.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_check_count_and_type(Md_Ast *ast, size_t c_count, NodeType typ) {
    if (ast->node_type != typ) {
        puts("NodeType mismatch");
        printf("\tWANTED: %d\n", typ);
        printf("\tGOT: %d\n", ast->node_type);
        assert(0 && "NodeType mismatch");
    }

    if (ast->c_count != c_count) {
        puts("c_count mismatch");
        printf("\tWANTED: %zu\n", c_count);
        printf("\tGOT: %zu\n", ast->c_count);
        assert(0 && "c_count mismatch");
    }
}

void test_match_text_node_text(Md_Ast *txt_node, const char *dest) {
    assert(txt_node->node_type == MD_TEXT_NODE);

    if (strcmp(txt_node->opt.text_opt.data, dest) != 0) {
        printf("\tWANTED: \"%s\"\n", dest);
        printf("\tGOT: \"%s\"\n", txt_node->opt.text_opt.data);
        assert(0 && "txt_node content did not match data");
    }
}

void test_md_heading() {
    Md_Ast *ast;

    /*************************************/
    puts("Test A");
    ast = kevlar_md_generate_ast("# Foo\n");

    test_check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "Foo");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test B");
    ast = kevlar_md_generate_ast("###");

    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test C");
    ast = kevlar_md_generate_ast("##### A");

    test_check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "A");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test D");
    ast = kevlar_md_generate_ast("########## Should NOT be a heading");

    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "########## Should NOT be a heading");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test E");
    ast = kevlar_md_generate_ast("## ");

    test_check_count_and_type(ast->children[0], 0, MD_HEADING_NODE);

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test F");
    ast = kevlar_md_generate_ast("### ## ### # ## # # # # ## ##### ");

    test_check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "## ### # ## # # # # ## ##### ");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test E");
    ast = kevlar_md_generate_ast("#\t\t\t\tHello");

    test_check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "Hello");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Heading with Unicode");
    ast = kevlar_md_generate_ast("# 你好世界 🌍");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    assert(ast->children[0]->opt.h_opt.level == 1);
    test_match_text_node_text(ast->children[0]->children[0], "你好世界 🌍");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Heading Levels Descending");
    ast = kevlar_md_generate_ast("# H1\n## H2\n### H3\n#### H4\n##### H5\n###### H6");
    test_check_count_and_type(ast, 6, MD_ROOT_NODE);
    for (int i = 0; i < 6; i++) {
        test_check_count_and_type(ast->children[i], 1, MD_HEADING_NODE);
        assert(ast->children[i]->opt.h_opt.level == i + 1);
    }
    kevlar_md_free_ast(ast);
    /*************************************/
}

void test_kevlar_md_find_next_occurrence() {
    char *data;
    size_t index;
    bool match;

    /*************************************/
    puts("Test A");

    data = "*Foo*";
    match = kevlar_md_find_next_occurrence(data, strlen(data), 1, "*", &index, false);

    assert(match);
    assert(index == 4);
    /*************************************/

    /*************************************/
    puts("Test B");

    data = "**Bar**";
    match = kevlar_md_find_next_occurrence(data, strlen(data), 2, "**", &index, false);

    assert(match);
    assert(index == 5);
    /*************************************/

    /*************************************/
    puts("Test C");

    data = "**Bar\n**";
    match = kevlar_md_find_next_occurrence(data, strlen(data), 2, "**", &index, true);

    assert(!match);
    /*************************************/

    /*************************************/
    puts("Test D");

    data = "~~Bar\n~~";
    match = kevlar_md_find_next_occurrence(data, strlen(data), 2, "~~", &index, false);

    assert(match);
    assert(index == 6);
    /*************************************/
}

void test_md_content() {
    Md_Ast *ast;

    /*************************************/
    puts("Test A");
    ast = kevlar_md_generate_ast("*Bedazzled*");
    /*
     * - Para
     * -- Em
     * --- Text
     */

    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "Bedazzled");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test B");
    ast = kevlar_md_generate_ast("Hello, *World*!");

    /*
     * - Para
     * -- text
     * -- em
     * --- text
     * -- text
     */

    test_check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "Hello, ");

    test_check_count_and_type(ast->children[0]->children[1], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[1]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[1]->children[0], "World");

    test_check_count_and_type(ast->children[0]->children[2], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[2], "!");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test C");
    ast = kevlar_md_generate_ast("*नमस्ते* दुनिया! 👋🌏");

    /*
     * - Para
     * -- em
     * --- text
     * -- text
     */

    test_check_count_and_type(ast->children[0], 2, MD_PARA_NODE);

    test_check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "नमस्ते");

    test_check_count_and_type(ast->children[0]->children[1], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[1], " दुनिया! 👋🌏");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test D");
    ast = kevlar_md_generate_ast("The _quick_ *brown* **fox** ***jumps*** ~~over~~ the lazy dog");

    // "The "
    // "quick"
    // " "
    // "brown"
    // " "
    // "fox"
    // " "
    // "jumps"
    // " "
    // "over"
    // " the lazy dog"

    test_check_count_and_type(ast->children[0], 11, MD_PARA_NODE);

    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "The ");

    test_check_count_and_type(ast->children[0]->children[1], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[1]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[1]->children[0], "quick");

    test_check_count_and_type(ast->children[0]->children[2], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[2], " ");

    test_check_count_and_type(ast->children[0]->children[3], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[3]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[3]->children[0], "brown");

    test_check_count_and_type(ast->children[0]->children[4], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[4], " ");

    test_check_count_and_type(ast->children[0]->children[5], 1, MD_STRONG_NODE);
    test_check_count_and_type(ast->children[0]->children[5]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[5]->children[0], "fox");

    test_check_count_and_type(ast->children[0]->children[2], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[2], " ");

    test_check_count_and_type(ast->children[0]->children[7], 1, MD_STRONG_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[7]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[7]->children[0], "jumps");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Nested Emphasis and Strong");
    ast = kevlar_md_generate_ast("**Bold with *nested emphasis* inside**");
    /*
     * - Para
     * -- Strong
     * --- Text "Bold with "
     * --- Em
     * ---- Text "nested emphasis"
     * --- Text " inside"
     */
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 3, MD_STRONG_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "Bold with ");
    test_check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[1]->children[0], 0,
                              MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[1]->children[0],
                              "nested emphasis");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Unclosed Emphasis");
    ast = kevlar_md_generate_ast("This has *unclosed emphasis");
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "This has *unclosed emphasis");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Strikethrough Alone");
    ast = kevlar_md_generate_ast("~~deleted text~~");
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_DEL_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "deleted text");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Strikethrough with Emphasis");
    ast = kevlar_md_generate_ast("~~deleted *with emphasis*~~");
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 2, MD_DEL_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "deleted ");
    test_check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[1]->children[0],
                              "with emphasis");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Empty String");
    ast = kevlar_md_generate_ast("");
    test_check_count_and_type(ast, 0, MD_ROOT_NODE);
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Plain Text Only");
    ast = kevlar_md_generate_ast("Just plain text with no formatting");
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "Just plain text with no formatting");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Unclosed Emphasis");
    ast = kevlar_md_generate_ast("This has *unclosed emphasis");
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "This has *unclosed emphasis");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Nested Emphasis and Strong");
    ast = kevlar_md_generate_ast("**Bold with *nested emphasis* inside**");
    /*
     * - Para
     * -- Strong
     * --- Text "Bold with "
     * --- Em
     * ---- Text "nested emphasis"
     * --- Text " inside"
     */
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 3, MD_STRONG_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "Bold with ");
    test_check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[1]->children[0], 0,
                              MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[1]->children[0],
                              "nested emphasis");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Strikethrough Alone");
    ast = kevlar_md_generate_ast("~~deleted text~~");
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_DEL_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "deleted text");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Strikethrough with Emphasis");
    ast = kevlar_md_generate_ast("~~deleted *with emphasis*~~");
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 2, MD_DEL_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "deleted ");
    test_check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[1]->children[0],
                              "with emphasis");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Special Characters in Text");
    ast = kevlar_md_generate_ast("Text with <html>, &amp;, \"quotes\", and 'apostrophes'");
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0],
                              "Text with <html>, &amp;, \"quotes\", and 'apostrophes'");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Asterisks Without Whitespace");
    ast = kevlar_md_generate_ast("*no space*immediately*adjacent*");
    test_check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "no space");
    test_check_count_and_type(ast->children[0]->children[1], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[1], "immediately");
    test_check_count_and_type(ast->children[0]->children[2], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[2]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[2]->children[0], "adjacent");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Underscore vs Asterisk Emphasis");
    ast = kevlar_md_generate_ast("_underscore_ and *asterisk* emphasis");
    test_check_count_and_type(ast->children[0], 4, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "underscore");
    test_check_count_and_type(ast->children[0]->children[2], 1, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[2]->children[0], "asterisk");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Triple Emphasis - StrongEm");
    ast = kevlar_md_generate_ast("***both bold and italic***");
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_STRONG_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "both bold and italic");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Mixed Triple Emphasis with Content");
    ast = kevlar_md_generate_ast("Start ***triple emphasis*** end");
    test_check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "Start ");
    test_check_count_and_type(ast->children[0]->children[1], 1, MD_STRONG_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[1]->children[0], "triple emphasis");
    test_match_text_node_text(ast->children[0]->children[2], " end");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Unicode Stress Test");
    ast = kevlar_md_generate_ast("*日本語*, **中文**, ***العربية***, ~~עִברִית~~");
    test_check_count_and_type(ast->children[0], 7, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "日本語");
    test_check_count_and_type(ast->children[0]->children[2], 1, MD_STRONG_NODE);
    test_match_text_node_text(ast->children[0]->children[2]->children[0], "中文");
    test_check_count_and_type(ast->children[0]->children[4], 1, MD_STRONG_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[4]->children[0], "العربية");
    test_check_count_and_type(ast->children[0]->children[6], 1, MD_DEL_NODE);
    test_match_text_node_text(ast->children[0]->children[6]->children[0], "עִברִית");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Emojis with Formatting");
    ast = kevlar_md_generate_ast("*🔥fire🔥* **💪strong💪** ***🚀rocket🚀***");
    test_check_count_and_type(ast->children[0], 5, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "🔥fire🔥");
    test_check_count_and_type(ast->children[0]->children[2], 1, MD_STRONG_NODE);
    test_match_text_node_text(ast->children[0]->children[2]->children[0], "💪strong💪");
    test_check_count_and_type(ast->children[0]->children[4], 1, MD_STRONG_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[4]->children[0], "🚀rocket🚀");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Escaped Asterisks");
    ast = kevlar_md_generate_ast("\\*not emphasis\\* but *this is*");
    test_check_count_and_type(ast->children[0], 2, MD_PARA_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "*not emphasis* but ");
    test_check_count_and_type(ast->children[0]->children[1], 1, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[1]->children[0], "this is");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Only Whitespace");
    ast = kevlar_md_generate_ast("     \t\t\t     ");
    test_check_count_and_type(ast->children[0], 0, MD_PARA_NODE);
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Long Text Performance");
    char long_text[10000];
    memset(long_text, 'a', 9999);
    long_text[9999] = '\0';
    ast = kevlar_md_generate_ast(long_text);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Deep Nesting");
    ast = kevlar_md_generate_ast("**strong *em ~~del _nested_ del~~ em* strong**");
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 3, MD_STRONG_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "strong ");
    test_check_count_and_type(ast->children[0]->children[0]->children[1], 3, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[1]->children[0], "em ");
    test_check_count_and_type(ast->children[0]->children[0]->children[1]->children[1], 3,
                              MD_DEL_NODE);
    kevlar_md_free_ast(ast);
    /*************************************/

    // /*************************************/
    // puts("Test: Unbalanced Tags Mixed");
    // ast = kevlar_md_generate_ast("*em **strong* still in strong?**");
    // // Should handle gracefully - behavior depends on implementation
    // test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    // kevlar_md_free_ast(ast);
    // /*************************************/

    /*************************************/
    puts("Test: Single Character Emphasis");
    ast = kevlar_md_generate_ast("*a* **b** ***c***");
    test_check_count_and_type(ast->children[0], 5, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "a");
    test_check_count_and_type(ast->children[0]->children[2], 1, MD_STRONG_NODE);
    test_match_text_node_text(ast->children[0]->children[2]->children[0], "b");
    test_check_count_and_type(ast->children[0]->children[4], 1, MD_STRONG_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[4]->children[0], "c");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Emphasis at Start and End");
    ast = kevlar_md_generate_ast("*start* middle **end**");
    test_check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "start");
    test_match_text_node_text(ast->children[0]->children[1], " middle ");
    test_check_count_and_type(ast->children[0]->children[2], 1, MD_STRONG_NODE);
    test_match_text_node_text(ast->children[0]->children[2]->children[0], "end");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Multiple Spaces Between Formatted");
    ast = kevlar_md_generate_ast("*em*     **strong**");
    test_check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[1], "     ");
    test_check_count_and_type(ast->children[0]->children[2], 1, MD_STRONG_NODE);
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Formatting with Numbers");
    ast = kevlar_md_generate_ast("*123* **456** ***789***");
    test_check_count_and_type(ast->children[0], 5, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "123");
    test_check_count_and_type(ast->children[0]->children[2], 1, MD_STRONG_NODE);
    test_match_text_node_text(ast->children[0]->children[2]->children[0], "456");
    test_check_count_and_type(ast->children[0]->children[4], 1, MD_STRONG_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[4]->children[0], "789");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Punctuation Heavy Text");
    ast = kevlar_md_generate_ast("*Hello!?* **What... the?** ***Really?!***");
    test_check_count_and_type(ast->children[0], 5, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "Hello!?");
    test_check_count_and_type(ast->children[0]->children[2], 1, MD_STRONG_NODE);
    test_match_text_node_text(ast->children[0]->children[2]->children[0], "What... the?");
    test_check_count_and_type(ast->children[0]->children[4], 1, MD_STRONG_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[4]->children[0], "Really?!");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Complex Document Structure");
    ast = kevlar_md_generate_ast("# Document Title\n\n"
                                 "This is the *introduction* with **bold** text.\n\n"
                                 "## First Section\n\n"
                                 "Content with ~~strikethrough~~ and ***bold italic***.\n"
                                 "Second line of same paragraph.\n\n"
                                 "### Subsection A\n\n"
                                 "More content here.\n\n"
                                 "### Subsection B\n\n"
                                 "Final paragraph with *formatting*.");

    test_check_count_and_type(ast, 8, MD_ROOT_NODE);

    test_check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    assert(ast->children[0]->opt.h_opt.level == 1);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "Document Title");

    test_check_count_and_type(ast->children[1], 5, MD_PARA_NODE);
    test_check_count_and_type(ast->children[1]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[1]->children[0], "This is the ");
    test_check_count_and_type(ast->children[1]->children[1], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[1]->children[1]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[1]->children[1]->children[0], "introduction");
    test_check_count_and_type(ast->children[1]->children[2], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[1]->children[2], " with ");
    test_check_count_and_type(ast->children[1]->children[3], 1, MD_STRONG_NODE);
    test_check_count_and_type(ast->children[1]->children[3]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[1]->children[3]->children[0], "bold");
    test_check_count_and_type(ast->children[1]->children[4], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[1]->children[4], " text.");

    test_check_count_and_type(ast->children[2], 1, MD_HEADING_NODE);
    assert(ast->children[2]->opt.h_opt.level == 2);
    test_check_count_and_type(ast->children[2]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[2]->children[0], "First Section");

    test_check_count_and_type(ast->children[3], 5, MD_PARA_NODE);
    test_check_count_and_type(ast->children[3]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[3]->children[0], "Content with ");
    test_check_count_and_type(ast->children[3]->children[1], 1, MD_DEL_NODE);
    test_check_count_and_type(ast->children[3]->children[1]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[3]->children[1]->children[0], "strikethrough");
    test_check_count_and_type(ast->children[3]->children[2], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[3]->children[2], " and ");
    test_check_count_and_type(ast->children[3]->children[3], 1, MD_STRONG_EM_NODE);
    test_check_count_and_type(ast->children[3]->children[3]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[3]->children[3]->children[0], "bold italic");
    test_check_count_and_type(ast->children[3]->children[4], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[3]->children[4], ".\nSecond line of same paragraph.");

    test_check_count_and_type(ast->children[4], 1, MD_HEADING_NODE);
    assert(ast->children[4]->opt.h_opt.level == 3);
    test_check_count_and_type(ast->children[4]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[4]->children[0], "Subsection A");

    test_check_count_and_type(ast->children[5], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[5]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[5]->children[0], "More content here.");

    test_check_count_and_type(ast->children[6], 1, MD_HEADING_NODE);
    assert(ast->children[6]->opt.h_opt.level == 3);
    test_check_count_and_type(ast->children[6]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[6]->children[0], "Subsection B");

    test_check_count_and_type(ast->children[7], 3, MD_PARA_NODE);
    test_check_count_and_type(ast->children[7]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[7]->children[0], "Final paragraph with ");
    test_check_count_and_type(ast->children[7]->children[1], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[7]->children[1]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[7]->children[1]->children[0], "formatting");
    test_check_count_and_type(ast->children[7]->children[2], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[7]->children[2], ".");

    kevlar_md_free_ast(ast);
    /*************************************/
}

void test_md_inline_code() {
    Md_Ast* ast;
#if 1
    /*************************************/
    puts("Test: Simple Inline Code");
    ast = kevlar_md_generate_ast("`code`");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
        test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "code");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Inline Code in Sentence");
    ast = kevlar_md_generate_ast("Use the `printf` function here.");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0], "Use the ");
        test_check_count_and_type(ast->children[0]->children[1], 1, MD_INLINE_CODE_BLOCK);
            test_check_count_and_type(ast->children[0]->children[1]->children[0], 0, MD_TEXT_NODE);
            test_match_text_node_text(ast->children[0]->children[1]->children[0], "printf");
        test_check_count_and_type(ast->children[0]->children[2], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[2], " function here.");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Multiple Inline Code Blocks");
    ast = kevlar_md_generate_ast("`foo` and `bar` and `baz`");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 5, MD_PARA_NODE);
        test_check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "foo");
        test_check_count_and_type(ast->children[0]->children[1], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[1], " and ");
        test_check_count_and_type(ast->children[0]->children[2], 1, MD_INLINE_CODE_BLOCK);
        test_match_text_node_text(ast->children[0]->children[2]->children[0], "bar");
        test_check_count_and_type(ast->children[0]->children[3], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[3], " and ");
        test_check_count_and_type(ast->children[0]->children[4], 1, MD_INLINE_CODE_BLOCK);
        test_match_text_node_text(ast->children[0]->children[4]->children[0], "baz");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block with Asterisks - No Formatting");
    ast = kevlar_md_generate_ast("`*not* **emphasis**`");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
        test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "*not* **emphasis**");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block with Tildes - No Strikethrough");
    ast = kevlar_md_generate_ast("`~~not deleted~~`");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
        test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "~~not deleted~~");
    kevlar_md_free_ast(ast);
    /*************************************/
#endif

    /*************************************/
    puts("Test: Escaped Backtick Outside Code");
    ast = kevlar_md_generate_ast("This is \\`not code\\` text.");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0], "This is `not code` text.");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Escaped Asterisk");
    ast = kevlar_md_generate_ast("\\*not emphasis\\*");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0], "*not emphasis*");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Escaped Underscore");
    ast = kevlar_md_generate_ast("\\_not emphasis\\_");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0], "_not emphasis_");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Escaped Tilde");
    ast = kevlar_md_generate_ast("\\~\\~not deleted\\~\\~");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0], "~~not deleted~~");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Escaped Hash");
    ast = kevlar_md_generate_ast("\\# Not a heading");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0], "# Not a heading");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Escaped Backslash");
    ast = kevlar_md_generate_ast("Double backslash: \\\\");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0], "Double backslash: \\");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Backslash Before Non-Special Character");
    ast = kevlar_md_generate_ast("\\a normal text");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
        // Backslash before non-special char should be literal backslash
        test_match_text_node_text(ast->children[0]->children[0], "\\a normal text");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code with Spaces");
    ast = kevlar_md_generate_ast("` spaced out `");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
        test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0]->children[0], " spaced out ");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Empty Code Block");
    ast = kevlar_md_generate_ast("``");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0], "``");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Unclosed Code Block");
    ast = kevlar_md_generate_ast("`unclosed code");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
        // Should treat as literal backtick if unclosed
        test_match_text_node_text(ast->children[0]->children[0], "`unclosed code");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code in Heading");
    ast = kevlar_md_generate_ast("# Heading with `code` inside");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 3, MD_HEADING_NODE);
    assert(ast->children[0]->opt.h_opt.level == 1);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0], "Heading with ");
        test_check_count_and_type(ast->children[0]->children[1], 1, MD_INLINE_CODE_BLOCK);
            test_check_count_and_type(ast->children[0]->children[1]->children[0], 0, MD_TEXT_NODE);
            test_match_text_node_text(ast->children[0]->children[1]->children[0], "code");
        test_check_count_and_type(ast->children[0]->children[2], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[2], " inside");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code with Emphasis Around It");
    ast = kevlar_md_generate_ast("*emphasis `code` more emphasis*");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 3, MD_EM_NODE);
        test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "emphasis ");
        test_check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_INLINE_CODE_BLOCK);
            test_check_count_and_type(ast->children[0]->children[0]->children[1]->children[0], 0, MD_TEXT_NODE);
            test_match_text_node_text(ast->children[0]->children[0]->children[1]->children[0], "code");
        test_check_count_and_type(ast->children[0]->children[0]->children[2], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0]->children[2], " more emphasis");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Adjacent Code Blocks");
    ast = kevlar_md_generate_ast("`first``second`");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 2, MD_PARA_NODE);
        test_check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "first");
        test_check_count_and_type(ast->children[0]->children[1], 1, MD_INLINE_CODE_BLOCK);
        test_match_text_node_text(ast->children[0]->children[1]->children[0], "second");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block with HTML Tags");
    ast = kevlar_md_generate_ast("`<div>HTML</div>`");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
        test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "<div>HTML</div>");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block with Special Characters");
    ast = kevlar_md_generate_ast("`!@#$%^&*()_+-=[]{}|;:',.<>?/`");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
        test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "!@#$%^&*()_+-=[]{}|;:',.<>?/");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Escaped Backtick in Code - Double Backtick Delimiter");
    ast = kevlar_md_generate_ast("``code with ` backtick``");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
        test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "code with ` backtick");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Mixed Escaping and Formatting");
    ast = kevlar_md_generate_ast("Text with \\*escaped\\* and *real* emphasis and `code`.");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 5, MD_PARA_NODE);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0], "Text with *escaped* and ");
        test_check_count_and_type(ast->children[0]->children[1], 1, MD_EM_NODE);
            test_check_count_and_type(ast->children[0]->children[1]->children[0], 0, MD_TEXT_NODE);
            test_match_text_node_text(ast->children[0]->children[1]->children[0], "real");
        test_check_count_and_type(ast->children[0]->children[2], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[2], " emphasis and ");
        test_check_count_and_type(ast->children[0]->children[3], 1, MD_INLINE_CODE_BLOCK);
            test_check_count_and_type(ast->children[0]->children[3]->children[0], 0, MD_TEXT_NODE);
            test_match_text_node_text(ast->children[0]->children[3]->children[0], "code");
        test_check_count_and_type(ast->children[0]->children[4], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[4], ".");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block Across Paragraphs - Should Not Span");
    ast = kevlar_md_generate_ast("`code start\n\ncode end`");
    // Double newline should break the code block
    test_check_count_and_type(ast, 2, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0], "`code start");
    test_check_count_and_type(ast->children[1], 1, MD_PARA_NODE);
        test_check_count_and_type(ast->children[1]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[1]->children[0], "code end`");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Complex Document with Code");
    ast = kevlar_md_generate_ast(
        "# Code Examples\n\n"
        "Use `printf(\"Hello\")` to print.\n\n"
        "**Important:** The `main()` function is *required*.\n\n"
        "## Escaping\n\n"
        "Use \\` to show a literal backtick."
    );
    

    test_check_count_and_type(ast, 5, MD_ROOT_NODE);
    
    test_check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    assert(ast->children[0]->opt.h_opt.level == 1);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[0], "Code Examples");
    
    test_check_count_and_type(ast->children[1], 3, MD_PARA_NODE);
        test_check_count_and_type(ast->children[1]->children[0], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[1]->children[0], "Use ");
        test_check_count_and_type(ast->children[1]->children[1], 1, MD_INLINE_CODE_BLOCK);
            test_check_count_and_type(ast->children[1]->children[1]->children[0], 0, MD_TEXT_NODE);
            test_match_text_node_text(ast->children[1]->children[1]->children[0], "printf(\"Hello\")");
        test_check_count_and_type(ast->children[1]->children[2], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[1]->children[2], " to print.");
    
    test_check_count_and_type(ast->children[2], 6, MD_PARA_NODE);
        test_check_count_and_type(ast->children[2]->children[0], 1, MD_STRONG_NODE);
            test_check_count_and_type(ast->children[2]->children[0]->children[0], 0, MD_TEXT_NODE);
            test_match_text_node_text(ast->children[2]->children[0]->children[0], "Important:");
        test_check_count_and_type(ast->children[2]->children[1], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[2]->children[1], " The ");
        test_check_count_and_type(ast->children[2]->children[2], 1, MD_INLINE_CODE_BLOCK);
            test_check_count_and_type(ast->children[2]->children[2]->children[0], 0, MD_TEXT_NODE);
            test_match_text_node_text(ast->children[2]->children[2]->children[0], "main()");
        test_check_count_and_type(ast->children[2]->children[3], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[2]->children[3], " function is ");
        test_check_count_and_type(ast->children[2]->children[4], 1, MD_EM_NODE);
            test_check_count_and_type(ast->children[2]->children[4]->children[0], 0, MD_TEXT_NODE);
            test_match_text_node_text(ast->children[2]->children[4]->children[0], "required");

        test_check_count_and_type(ast->children[2]->children[5], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[2]->children[5], ".");
    
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Unicode in Code");
    ast = kevlar_md_generate_ast("`日本語` and `🚀emoji🔥`");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
        test_check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "日本語");
        test_check_count_and_type(ast->children[0]->children[1], 0, MD_TEXT_NODE);
        test_match_text_node_text(ast->children[0]->children[1], " and ");
        test_check_count_and_type(ast->children[0]->children[2], 1, MD_INLINE_CODE_BLOCK);
        test_match_text_node_text(ast->children[0]->children[2]->children[0], "🚀emoji🔥");
    kevlar_md_free_ast(ast);
    /*************************************/
}

void test_md_basic() {
    Md_Ast *ast = kevlar_md_generate_ast("Hello, World!");

    assert(ast->node_type == MD_ROOT_NODE && "AST must have a root node");

    kevlar_md_free_ast(ast);
}

void test_markdown() {

#if 1
    puts("INFO: test_md_basic()");
    test_md_basic();
    puts("SUCCESS: test_md_basic()");

    puts("INFO: test_md_heading()");
    test_md_heading();
    puts("SUCCESS: test_md_heading()");

    puts("INFO: test_kevlar_md_find_next_occurrence()");
    test_kevlar_md_find_next_occurrence();
    puts("SUCCESS: test_kevlar_md_find_next_occurrence()");

    puts("INFO: test_md_content()");
    test_md_content();
    puts("SUCCESS: test_md_content()");
#endif

    puts("INFO: test_md_inline_code(()");
    test_md_inline_code();
    puts("SUCCESS: test_md_inline_code(()");
};
