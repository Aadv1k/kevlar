#include "../src/kevlar_markdown.h"
#include "../src/utils.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_check_count_and_type(Md_Ast *ast, size_t c_count, NodeType typ) {
    if (ast->node_type != typ) {
        puts("NodeType mismatch");
        printf("\tWANTED: %s\n", utl_node_type_to_str(typ));
        printf("\tGOT: %s\n", utl_node_type_to_str(ast->node_type));
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

// void test_kevlar_md_find_next_occurrence() {
//     char *data;
//     size_t index;
//     bool match;

//     /*************************************/
//     puts("Test A");

//     data = "*Foo*";
//     match = kevlar_md_find_next_occurrence(data, strlen(data), 1, "*", &index, false);

//     assert(match);
//     assert(index == 4);
//     /*************************************/

//     /*************************************/
//     puts("Test B");

//     data = "**Bar**";
//     match = kevlar_md_find_next_occurrence(data, strlen(data), 2, "**", &index, false);

//     assert(match);
//     assert(index == 5);
//     /*************************************/

//     /*************************************/
//     puts("Test C");

//     data = "**Bar\n**";
//     match = kevlar_md_find_next_occurrence(data, strlen(data), 2, "**", &index, true);

//     assert(!match);
//     /*************************************/

//     /*************************************/
//     puts("Test D");

//     data = "~~Bar\n~~";
//     match = kevlar_md_find_next_occurrence(data, strlen(data), 2, "~~", &index, false);

//     assert(match);
//     assert(index == 6);
//     /*************************************/
// }

void test_md_content() {
    Md_Ast *ast;
#if 0
    /*************************************/
    puts("Test A");
    ast = kevlar_md_generate_ast("*Be *dazz* led*");

    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 3, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "Be ");

    test_check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[1]->children[0], 0,
                              MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[1]->children[0], "dazz");

    test_check_count_and_type(ast->children[0]->children[0]->children[2], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[2], " led");

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
    puts("Test C/1");
    ast = kevlar_md_generate_ast("The **quick *brown* fox** jumps __over the lazy dog__");
    test_check_count_and_type(ast->children[0], 4, MD_PARA_NODE);

    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "The ");

    test_check_count_and_type(ast->children[0]->children[1], 3, MD_STRONG_NODE);
    /**/ test_check_count_and_type(ast->children[0]->children[1]->children[0], 0, MD_TEXT_NODE);
    /**/ test_match_text_node_text(ast->children[0]->children[1]->children[0], "quick ");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test C/2");
    ast = kevlar_md_generate_ast("**_Hello World_**");

    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_STRONG_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0]->children[0], "Hello World");

    kevlar_md_free_ast(ast);
    /*************************************/

#endif

    /*************************************/
    puts("Test C/3");
    ast = kevlar_md_generate_ast("***A***");

    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 1, MD_STRONG_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0]->children[0], 0,
                              MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0]->children[0], "A");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test D");
    ast = kevlar_md_generate_ast("The _quick_ *brown* **fox** ***jumps*** over the lazy dog");

    // "The "
    // "quick"
    // " "
    // "brown"
    // " "
    // "fox"
    // " "
    // "jumps"
    // " over the lazy dog"

    test_check_count_and_type(ast->children[0], 9, MD_PARA_NODE);

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

    test_check_count_and_type(ast->children[0]->children[7], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[7]->children[0], 1, MD_STRONG_NODE);
    test_check_count_and_type(ast->children[0]->children[7]->children[0]->children[0], 0,
                              MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[7]->children[0]->children[0], "jumps");

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

    // /*************************************/
    // puts("Test: Strikethrough Alone");
    // ast = kevlar_md_generate_ast("~~deleted text~~");
    // test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    // test_check_count_and_type(ast->children[0]->children[0], 1, MD_DEL_NODE);
    // test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
    // test_match_text_node_text(ast->children[0]->children[0]->children[0], "deleted text");
    // kevlar_md_free_ast(ast);
    // /*************************************/

    // /*************************************/
    // puts("Test: Strikethrough with Emphasis");
    // ast = kevlar_md_generate_ast("~~deleted *with emphasis*~~");
    // test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    // test_check_count_and_type(ast->children[0]->children[0], 2, MD_DEL_NODE);
    // test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
    // test_match_text_node_text(ast->children[0]->children[0]->children[0], "deleted ");
    // test_check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_EM_NODE);
    // test_match_text_node_text(ast->children[0]->children[0]->children[1]->children[0],
    //                           "with emphasis");
    // kevlar_md_free_ast(ast);
    // /*************************************/

    // /*************************************/
    // puts("Test: Empty String");
    // ast = kevlar_md_generate_ast("");
    // test_check_count_and_type(ast, 0, MD_ROOT_NODE);
    // kevlar_md_free_ast(ast);
    // /*************************************/

    // /*************************************/
    // puts("Test: Plain Text Only");
    // ast = kevlar_md_generate_ast("Just plain text with no formatting");
    // test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    // test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    // test_match_text_node_text(ast->children[0]->children[0], "Just plain text with no
    // formatting"); kevlar_md_free_ast(ast);
    // /*************************************/

    // /*************************************/
    // puts("Test: Unclosed Emphasis");
    // ast = kevlar_md_generate_ast("This has *unclosed emphasis");
    // test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    // test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    // test_match_text_node_text(ast->children[0]->children[0], "This has *unclosed emphasis");
    // kevlar_md_free_ast(ast);
    // /*************************************/

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
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 1, MD_STRONG_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0]->children[0], 0,
                              MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0]->children[0],
                              "both bold and italic");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Mixed Triple Emphasis with Content");
    ast = kevlar_md_generate_ast("Start ***triple emphasis*** end");
    test_check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "Start ");
    test_check_count_and_type(ast->children[0]->children[1], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[1]->children[0], 1, MD_STRONG_NODE);
    test_check_count_and_type(ast->children[0]->children[1]->children[0]->children[0], 0,
                              MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[1]->children[0]->children[0],
                              "triple emphasis");
    test_match_text_node_text(ast->children[0]->children[2], " end");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Unicode Stress Test");
    ast = kevlar_md_generate_ast("*日本語*, **中文**, ***العربية***, עִברִית");

    test_check_count_and_type(ast->children[0], 6, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "日本語");
    test_check_count_and_type(ast->children[0]->children[2], 1, MD_STRONG_NODE);
    test_match_text_node_text(ast->children[0]->children[2]->children[0], "中文");

    test_check_count_and_type(ast->children[0]->children[4], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[4]->children[0], 1, MD_STRONG_NODE);
    test_check_count_and_type(ast->children[0]->children[4]->children[0]->children[0], 0,
                              MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[4]->children[0]->children[0], "العربية");
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

    test_check_count_and_type(ast->children[0]->children[4], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[4]->children[0], 1, MD_STRONG_NODE);
    test_check_count_and_type(ast->children[0]->children[4]->children[0]->children[0], 0,
                              MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[4]->children[0]->children[0],
                              "🚀rocket🚀");

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
    Md_Ast *ast;
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
    test_check_count_and_type(ast->children[0]->children[0]->children[1]->children[0], 0,
                              MD_TEXT_NODE);
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
    test_match_text_node_text(ast->children[0]->children[0]->children[0],
                              "!@#$%^&*()_+-=[]{}|;:',.<>?/");
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
    ast = kevlar_md_generate_ast("# Code Examples\n\n"
                                 "Use `printf(\"Hello\")` to print.\n\n"
                                 "**Important:** The `main()` function is *required*.\n\n"
                                 "## Escaping\n\n"
                                 "Use \\` to show a literal backtick.");

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

void test_md_links() {
    Md_Ast *ast;

#if 1
    /*************************************/
    puts("Test: Simple Link");
    ast = kevlar_md_generate_ast("[link text](https://example.com)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("https://example.com"));
    assert(strncmp(ast->children[0]->children[0]->opt.link_opt.href_str, "https://example.com",
                   ast->children[0]->children[0]->opt.link_opt.href_len) == 0);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "link text");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link in Sentence");
    ast = kevlar_md_generate_ast("Check out [this site](https://example.com) for more info.");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "Check out ");
    test_check_count_and_type(ast->children[0]->children[1], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[1]->opt.link_opt.href_len == strlen("https://example.com"));
    assert(strncmp(ast->children[0]->children[1]->opt.link_opt.href_str, "https://example.com",
                   ast->children[0]->children[1]->opt.link_opt.href_len) == 0);
    test_check_count_and_type(ast->children[0]->children[1]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[1]->children[0], "this site");
    test_check_count_and_type(ast->children[0]->children[2], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[2], " for more info.");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Multiple Links");
    ast = kevlar_md_generate_ast("[first](url1) and [second](url2) links.");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 4, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url1"));
    assert(strncmp(ast->children[0]->children[0]->opt.link_opt.href_str, "url1",
                   ast->children[0]->children[0]->opt.link_opt.href_len) == 0);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "first");
    test_check_count_and_type(ast->children[0]->children[1], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[1], " and ");
    test_check_count_and_type(ast->children[0]->children[2], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[2]->opt.link_opt.href_len == strlen("url2"));
    assert(strncmp(ast->children[0]->children[2]->opt.link_opt.href_str, "url2",
                   ast->children[0]->children[2]->opt.link_opt.href_len) == 0);
    test_match_text_node_text(ast->children[0]->children[2]->children[0], "second");
    test_check_count_and_type(ast->children[0]->children[3], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[3], " links.");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link with Emphasis in Text");
    ast = kevlar_md_generate_ast("[*emphasized* text](url)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 2, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url"));
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0]->children[0], 0,
                              MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0]->children[0],
                              "emphasized");
    test_check_count_and_type(ast->children[0]->children[0]->children[1], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[1], " text");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link with Strong in Text");
    ast = kevlar_md_generate_ast("[**bold** link](url)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 2, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url"));
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 1, MD_STRONG_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0]->children[0], "bold");
    test_check_count_and_type(ast->children[0]->children[0]->children[1], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[1], " link");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link with Code in Text");
    ast = kevlar_md_generate_ast("[see `code` here](url)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 3, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url"));
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "see ");
    test_check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_INLINE_CODE_BLOCK);
    test_match_text_node_text(ast->children[0]->children[0]->children[1]->children[0], "code");
    test_check_count_and_type(ast->children[0]->children[0]->children[2], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[2], " here");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link with Multiple Formatting");
    ast = kevlar_md_generate_ast("[*em* **strong** `code`](url)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 5, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url"));
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 1, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0]->children[0], "em");
    test_check_count_and_type(ast->children[0]->children[0]->children[1], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[1], " ");
    test_check_count_and_type(ast->children[0]->children[0]->children[2], 1, MD_STRONG_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[2]->children[0], "strong");
    test_check_count_and_type(ast->children[0]->children[0]->children[3], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[3], " ");
    test_check_count_and_type(ast->children[0]->children[0]->children[4], 1, MD_INLINE_CODE_BLOCK);
    test_match_text_node_text(ast->children[0]->children[0]->children[4]->children[0], "code");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Emphasis Around Link");
    ast = kevlar_md_generate_ast("*emphasis [link](url) text*");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 3, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "emphasis ");
    test_check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->children[1]->opt.link_opt.href_len == strlen("url"));
    test_match_text_node_text(ast->children[0]->children[0]->children[1]->children[0], "link");
    test_check_count_and_type(ast->children[0]->children[0]->children[2], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[2], " text");
    kevlar_md_free_ast(ast);
    /*************************************/

    // /*************************************/
    // puts("Test: Link with Empty URL");
    // ast = kevlar_md_generate_ast("[link text]()");
    // test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    // test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    // test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    //     assert(ast->children[0]->children[0]->opt.link_opt.href_len == 0);
    //     test_match_text_node_text(ast->children[0]->children[0]->children[0], "link text");
    // kevlar_md_free_ast(ast);
    // /*************************************/

    /*************************************/
    puts("Test: Link with Spaces in URL");
    ast = kevlar_md_generate_ast("[text](https://example.com/path with spaces)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len ==
           strlen("https://example.com/path with spaces"));
    assert(strncmp(ast->children[0]->children[0]->opt.link_opt.href_str,
                   "https://example.com/path with spaces",
                   ast->children[0]->children[0]->opt.link_opt.href_len) == 0);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "text");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link with Query Parameters");
    ast = kevlar_md_generate_ast("[text](https://example.com?foo=bar&baz=qux)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len ==
           strlen("https://example.com?foo=bar&baz=qux"));
    assert(strncmp(ast->children[0]->children[0]->opt.link_opt.href_str,
                   "https://example.com?foo=bar&baz=qux",
                   ast->children[0]->children[0]->opt.link_opt.href_len) == 0);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "text");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link with Fragment");
    ast = kevlar_md_generate_ast("[jump](#section)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("#section"));
    assert(strncmp(ast->children[0]->children[0]->opt.link_opt.href_str, "#section",
                   ast->children[0]->children[0]->opt.link_opt.href_len) == 0);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "jump");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Relative URL");
    ast = kevlar_md_generate_ast("[relative](../path/to/file.html)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("../path/to/file.html"));
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "relative");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Mailto Link");
    ast = kevlar_md_generate_ast("[email](mailto:user@example.com)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len ==
           strlen("mailto:user@example.com"));
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "email");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Adjacent Links");
    ast = kevlar_md_generate_ast("[first](url1)[second](url2)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 2, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url1"));
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "first");
    test_check_count_and_type(ast->children[0]->children[1], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[1]->opt.link_opt.href_len == strlen("url2"));
    test_match_text_node_text(ast->children[0]->children[1]->children[0], "second");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Unclosed Link Bracket");
    ast = kevlar_md_generate_ast("[unclosed link(url)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "[unclosed link(url)");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Unclosed Link Parenthesis");
    ast = kevlar_md_generate_ast("[text](unclosed");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "[text](unclosed");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Missing URL Parentheses");
    ast = kevlar_md_generate_ast("[text]url");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "[text]url");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Space Between Bracket and Parenthesis");
    ast = kevlar_md_generate_ast("[text] (url)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "[text] (url)");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Escaped Bracket in Link Text");
    ast = kevlar_md_generate_ast("[text \\[with bracket\\]](url)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url"));
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "text [with bracket]");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Escaped Parenthesis in URL");
    ast = kevlar_md_generate_ast("[text](url\\(with\\)parens)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url(with)parens"));
    assert(strncmp(ast->children[0]->children[0]->opt.link_opt.href_str, "url(with)parens",
                   ast->children[0]->children[0]->opt.link_opt.href_len) == 0);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "text");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link in Heading");
    ast = kevlar_md_generate_ast("# Heading with [link](url)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 2, MD_HEADING_NODE);
    assert(ast->children[0]->opt.h_opt.level == 1);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "Heading with ");
    test_check_count_and_type(ast->children[0]->children[1], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[1]->opt.link_opt.href_len == strlen("url"));
    test_match_text_node_text(ast->children[0]->children[1]->children[0], "link");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link with Newline in Text - Soft Break");
    ast = kevlar_md_generate_ast("[link\ntext](url)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url"));
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "link\ntext");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link Cannot Span Paragraphs");
    ast = kevlar_md_generate_ast("[text\n\nmore](url)");
    test_check_count_and_type(ast, 2, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "[text");
    test_check_count_and_type(ast->children[1], 1, MD_PARA_NODE);
    test_match_text_node_text(ast->children[1]->children[0], "more](url)");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Unicode in Link Text");
    ast = kevlar_md_generate_ast("[日本語 🌏](url)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url"));
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "日本語 🌏");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Unicode in URL");
    ast = kevlar_md_generate_ast("[text](https://例え.jp/パス)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("https://例え.jp/パス"));
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "text");
    kevlar_md_free_ast(ast);
    /*************************************/

    // TODO: this requires a stack based parser which we do not have as of yet
    // /*************************************/
    // puts("Test: Link with Parentheses in URL - Unescaped");
    // ast =
    // kevlar_md_generate_ast("[text](https://en.wikipedia.org/wiki/C_(programming_language))");
    // utl_visualize_ast(ast, 0);
    // test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    // test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    // test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    //     // Parser should handle balanced parens in URL
    //     assert(ast->children[0]->children[0]->opt.link_opt.href_len ==
    //     strlen("https://en.wikipedia.org/wiki/C_(programming_language)"));
    //     test_match_text_node_text(ast->children[0]->children[0]->children[0], "text");
    // kevlar_md_free_ast(ast);
    // /*************************************/

    /*************************************/
    puts("Test: Long URL");
    ast = kevlar_md_generate_ast("[link](https://example.com/very/long/path/with/many/segments/and/"
                                 "query?param1=value1&param2=value2&param3=value3#fragment)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len > 50);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "link");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Complex Document with Links");
    ast =
        kevlar_md_generate_ast("# Documentation\n\n"
                               "See the [official docs](https://docs.example.com) for details.\n\n"
                               "## Resources\n\n"
                               "Check [*this* guide](url1) and [**that** tutorial](url2).\n\n"
                               "Contact us at [support@example.com](mailto:support@example.com).");

    test_check_count_and_type(ast, 5, MD_ROOT_NODE);

    test_check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    assert(ast->children[0]->opt.h_opt.level == 1);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "Documentation");

    // Para: See the [official docs]...
    test_check_count_and_type(ast->children[1], 3, MD_PARA_NODE);
    test_check_count_and_type(ast->children[1]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[1]->children[0], "See the ");
    test_check_count_and_type(ast->children[1]->children[1], 1, MD_LINK_NODE);
    assert(ast->children[1]->children[1]->opt.link_opt.href_len ==
           strlen("https://docs.example.com"));
    test_check_count_and_type(ast->children[1]->children[1]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[1]->children[1]->children[0], "official docs");
    test_check_count_and_type(ast->children[1]->children[2], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[1]->children[2], " for details.");

    // H2: Resources
    test_check_count_and_type(ast->children[2], 1, MD_HEADING_NODE);
    assert(ast->children[2]->opt.h_opt.level == 2);
    test_check_count_and_type(ast->children[2]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[2]->children[0], "Resources");

    // Para: Check [*this* guide]... and [**that** tutorial]... Contact us at [email]...
    test_check_count_and_type(ast->children[3], 5, MD_PARA_NODE);
    test_check_count_and_type(ast->children[3]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[3]->children[0], "Check ");

    test_check_count_and_type(ast->children[3]->children[1], 2, MD_LINK_NODE);
    assert(ast->children[3]->children[1]->opt.link_opt.href_len == strlen("url1"));
    test_check_count_and_type(ast->children[3]->children[1]->children[0], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[3]->children[1]->children[0]->children[0], 0,
                              MD_TEXT_NODE);
    test_match_text_node_text(ast->children[3]->children[1]->children[0]->children[0], "this");
    test_check_count_and_type(ast->children[3]->children[1]->children[1], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[3]->children[1]->children[1], " guide");

    test_check_count_and_type(ast->children[3]->children[2], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[3]->children[2], " and ");

    test_check_count_and_type(ast->children[3]->children[3], 2, MD_LINK_NODE);
    assert(ast->children[3]->children[3]->opt.link_opt.href_len == strlen("url2"));
    test_check_count_and_type(ast->children[3]->children[3]->children[0], 1, MD_STRONG_NODE);
    test_check_count_and_type(ast->children[3]->children[3]->children[0]->children[0], 0,
                              MD_TEXT_NODE);
    test_match_text_node_text(ast->children[3]->children[3]->children[0]->children[0], "that");
    test_check_count_and_type(ast->children[3]->children[3]->children[1], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[3]->children[3]->children[1], " tutorial");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link After Code");
    ast = kevlar_md_generate_ast("`code` followed by [link](url)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_INLINE_CODE_BLOCK);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "code");
    test_check_count_and_type(ast->children[0]->children[1], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[1], " followed by ");
    test_check_count_and_type(ast->children[0]->children[2], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[2]->opt.link_opt.href_len == strlen("url"));
    test_match_text_node_text(ast->children[0]->children[2]->children[0], "link");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link Before Code");
    ast = kevlar_md_generate_ast("[link](url) followed by `code`");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 3, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url"));
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "link");
    test_check_count_and_type(ast->children[0]->children[1], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[1], " followed by ");
    test_check_count_and_type(ast->children[0]->children[2], 1, MD_INLINE_CODE_BLOCK);
    test_match_text_node_text(ast->children[0]->children[2]->children[0], "code");
    kevlar_md_free_ast(ast);
    /*************************************/
#endif

    /*************************************/
    puts("Test: Link with Strikethrough in Text");
    ast = kevlar_md_generate_ast("[~~deleted~~ text](url)");
    // utl_visualize_ast(ast, 0);
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 2, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url"));
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 1, MD_DEL_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0]->children[0], 0,
                              MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0]->children[0], "deleted");
    test_check_count_and_type(ast->children[0]->children[0]->children[1], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[1], " text");
    kevlar_md_free_ast(ast);
    /*************************************/
#if 1
    /*************************************/
    puts("Test: Multiple Links in Heading");
    ast = kevlar_md_generate_ast("## Section [one](url1) and [two](url2)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 4, MD_HEADING_NODE);
    assert(ast->children[0]->opt.h_opt.level == 2);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "Section ");
    test_check_count_and_type(ast->children[0]->children[1], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[1]->opt.link_opt.href_len == strlen("url1"));
    test_match_text_node_text(ast->children[0]->children[1]->children[0], "one");
    test_check_count_and_type(ast->children[0]->children[2], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[2], " and ");
    test_check_count_and_type(ast->children[0]->children[3], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[3]->opt.link_opt.href_len == strlen("url2"));
    test_match_text_node_text(ast->children[0]->children[3]->children[0], "two");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link with All Formatting Types");
    ast = kevlar_md_generate_ast("[*em* **strong** ***both*** ~~del~~ `code`](url)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 9, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url"));

    test_check_count_and_type(ast->children[0]->children[0]->children[0], 1, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0]->children[0], "em");
    test_check_count_and_type(ast->children[0]->children[0]->children[1], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[1], " ");

    test_check_count_and_type(ast->children[0]->children[0]->children[2], 1, MD_STRONG_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[2]->children[0], "strong");
    test_check_count_and_type(ast->children[0]->children[0]->children[3], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[3], " ");

    test_check_count_and_type(ast->children[0]->children[0]->children[4], 1, MD_STRONG_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[4]->children[0], "both");
    test_check_count_and_type(ast->children[0]->children[0]->children[5], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[5], " ");

    test_check_count_and_type(ast->children[0]->children[0]->children[6], 1, MD_DEL_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[6]->children[0], "del");
    test_check_count_and_type(ast->children[0]->children[0]->children[7], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[7], " ");

    test_check_count_and_type(ast->children[0]->children[0]->children[8], 1, MD_INLINE_CODE_BLOCK);
    test_match_text_node_text(ast->children[0]->children[0]->children[8]->children[0], "code");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: URL with Special Characters");
    ast = kevlar_md_generate_ast("[link](https://example.com/path?q=%20%21%40%23)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len ==
           strlen("https://example.com/path?q=%20%21%40%23"));
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "link");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Data URL");
    ast = kevlar_md_generate_ast("[img](data:image/png;base64,iVBORw0KGgo=)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len ==
           strlen("data:image/png;base64,iVBORw0KGgo="));
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "img");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: FTP URL");
    ast = kevlar_md_generate_ast("[file](ftp://ftp.example.com/file.txt)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len ==
           strlen("ftp://ftp.example.com/file.txt"));
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "file");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Just Fragment URL");
    ast = kevlar_md_generate_ast("[top](#)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == 1);
    assert(strncmp(ast->children[0]->children[0]->opt.link_opt.href_str, "#", 1) == 0);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "top");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link with Only Whitespace in Text");
    ast = kevlar_md_generate_ast("[   ](url)");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_LINK_NODE);
    assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url"));
    kevlar_md_free_ast(ast);
    /*************************************/
#endif
}

void test_md_code_block() {
    Md_Ast *ast;

    /*************************************/
    puts("Test: Simple Code Block");
    ast = kevlar_md_generate_ast("```\ncode\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    assert(ast->children[0]->opt.code_opt.lang_str == NULL ||
           ast->children[0]->opt.code_opt.lang_str_len == 0);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "code");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block with Language");
    ast = kevlar_md_generate_ast("```python\nprint('hello')\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    assert(ast->children[0]->opt.code_opt.lang_str_len == 6);
    assert(strncmp(ast->children[0]->opt.code_opt.lang_str, "python", 6) == 0);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "print('hello')");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block with Multiple Lines");
    ast = kevlar_md_generate_ast("```\nline1\nline2\nline3\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "line1\nline2\nline3");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block Preserves Indentation");
    ast = kevlar_md_generate_ast("```\n    indented\n        more indented\n    back\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0],
                              "    indented\n        more indented\n    back");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block with Empty Lines");
    ast = kevlar_md_generate_ast("```\nline1\n\nline3\n```");

    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "line1\n\nline3");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block Before Paragraph");
    ast = kevlar_md_generate_ast("```\ncode\n```\n\nParagraph text.");
    test_check_count_and_type(ast, 2, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    test_match_text_node_text(ast->children[0]->children[0], "code");
    test_check_count_and_type(ast->children[1], 1, MD_PARA_NODE);
    test_match_text_node_text(ast->children[1]->children[0], "Paragraph text.");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Paragraph Before Code Block");
    ast = kevlar_md_generate_ast("Paragraph text.\n\n```\ncode\n```");
    test_check_count_and_type(ast, 2, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "Paragraph text.");
    test_check_count_and_type(ast->children[1], 1, MD_CODE_BLOCK);
    test_match_text_node_text(ast->children[1]->children[0], "code");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block with Markdown - Not Parsed");
    ast = kevlar_md_generate_ast("```\n*not emphasis*\n**not strong**\n[not link](url)\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0],
                              "*not emphasis*\n**not strong**\n[not link](url)");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block with Backticks Inside");
    ast = kevlar_md_generate_ast("```\ncode with ` backtick\nand ``` three\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "code with ` backtick\nand ``` three");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block JavaScript");
    ast = kevlar_md_generate_ast("```javascript\nconst x = 42;\nconsole.log(x);\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    assert(ast->children[0]->opt.code_opt.lang_str_len == 10);
    assert(strncmp(ast->children[0]->opt.code_opt.lang_str, "javascript", 10) == 0);
    test_match_text_node_text(ast->children[0]->children[0], "const x = 42;\nconsole.log(x);");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block C");
    ast = kevlar_md_generate_ast("```c\nint main() {\n    return 0;\n}\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    assert(ast->children[0]->opt.code_opt.lang_str_len == 1);
    assert(strncmp(ast->children[0]->opt.code_opt.lang_str, "c", 1) == 0);
    test_match_text_node_text(ast->children[0]->children[0], "int main() {\n    return 0;\n}");
    kevlar_md_free_ast(ast);
    /*************************************/

    // /*************************************/
    // puts("Test: Code Block Empty");
    // ast = kevlar_md_generate_ast("```\n```");
    // test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    // test_check_count_and_type(ast->children[0], 0, MD_CODE_BLOCK);
    // kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Unclosed Code Block");
    ast = kevlar_md_generate_ast("```\nunclosed code");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Four Backticks - Not Standard");
    ast = kevlar_md_generate_ast("````\ncode\n````");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    // Implementation specific - might work or treat as paragraph
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block Not at Start of Line");
    ast = kevlar_md_generate_ast("Text ```\ncode\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    // Should NOT be code block if not at line start
    test_match_text_node_text(ast->children[0]->children[0], "Text ```\ncode\n```");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block with Heading Inside");
    ast = kevlar_md_generate_ast("```\n# Not a heading\n## Also not\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    test_match_text_node_text(ast->children[0]->children[0], "# Not a heading\n## Also not");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Multiple Code Blocks");
    ast = kevlar_md_generate_ast("```\nblock1\n```\n\n```\nblock2\n```");
    test_check_count_and_type(ast, 2, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    test_match_text_node_text(ast->children[0]->children[0], "block1");
    test_check_count_and_type(ast->children[1], 1, MD_CODE_BLOCK);
    test_match_text_node_text(ast->children[1]->children[0], "block2");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block with Different Languages");
    ast = kevlar_md_generate_ast("```python\ncode1\n```\n\n```java\ncode2\n```");
    test_check_count_and_type(ast, 2, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    assert(ast->children[0]->opt.code_opt.lang_str_len == 6);
    assert(strncmp(ast->children[0]->opt.code_opt.lang_str, "python", 6) == 0);
    test_match_text_node_text(ast->children[0]->children[0], "code1");
    test_check_count_and_type(ast->children[1], 1, MD_CODE_BLOCK);
    assert(ast->children[1]->opt.code_opt.lang_str_len == 4);
    assert(strncmp(ast->children[1]->opt.code_opt.lang_str, "java", 4) == 0);
    test_match_text_node_text(ast->children[1]->children[0], "code2");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block Between Headings");
    ast = kevlar_md_generate_ast("# Title\n\n```\ncode\n```\n\n## Section");
    test_check_count_and_type(ast, 3, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    assert(ast->children[0]->opt.h_opt.level == 1);
    test_check_count_and_type(ast->children[1], 1, MD_CODE_BLOCK);
    test_match_text_node_text(ast->children[1]->children[0], "code");
    test_check_count_and_type(ast->children[2], 1, MD_HEADING_NODE);
    assert(ast->children[2]->opt.h_opt.level == 2);
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block with HTML");
    ast = kevlar_md_generate_ast("```html\n<div class=\"container\">\n  <p>Hello</p>\n</div>\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    assert(ast->children[0]->opt.code_opt.lang_str_len == 4);
    assert(strncmp(ast->children[0]->opt.code_opt.lang_str, "html", 4) == 0);
    test_match_text_node_text(ast->children[0]->children[0],
                              "<div class=\"container\">\n  <p>Hello</p>\n</div>");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block with Special Characters");
    ast = kevlar_md_generate_ast("```\n!@#$%^&*()_+-=[]{}|;':\",./<>?\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    test_match_text_node_text(ast->children[0]->children[0], "!@#$%^&*()_+-=[]{}|;':\",./<>?");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block with Unicode");
    ast = kevlar_md_generate_ast("```\n日本語\n中文\nעברית\n🚀🔥\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    test_match_text_node_text(ast->children[0]->children[0], "日本語\n中文\nעברית\n🚀🔥");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block with Tabs");
    ast = kevlar_md_generate_ast("```\nline\twith\ttabs\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    test_match_text_node_text(ast->children[0]->children[0], "line\twith\ttabs");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block SQL");
    ast = kevlar_md_generate_ast("```sql\nSELECT * FROM users WHERE id = 1;\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    assert(ast->children[0]->opt.code_opt.lang_str_len == 3);
    assert(strncmp(ast->children[0]->opt.code_opt.lang_str, "sql", 3) == 0);
    test_match_text_node_text(ast->children[0]->children[0], "SELECT * FROM users WHERE id = 1;");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block Bash");
    ast = kevlar_md_generate_ast("```bash\n#!/bin/bash\necho \"Hello\"\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    assert(ast->children[0]->opt.code_opt.lang_str_len == 4);
    assert(strncmp(ast->children[0]->opt.code_opt.lang_str, "bash", 4) == 0);
    test_match_text_node_text(ast->children[0]->children[0], "#!/bin/bash\necho \"Hello\"");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block Trailing Newline Handling");
    ast = kevlar_md_generate_ast("```\ncode\n\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    // May or may not include trailing newline - depends on spec
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block Language with Dash");
    ast = kevlar_md_generate_ast("```c-sharp\nvar x = 10;\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    assert(ast->children[0]->opt.code_opt.lang_str_len == 7);
    assert(strncmp(ast->children[0]->opt.code_opt.lang_str, "c-sharp", 7) == 0);
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block Language with Plus");
    ast = kevlar_md_generate_ast("```c++\nint x = 10;\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    assert(ast->children[0]->opt.code_opt.lang_str_len == 3);
    assert(strncmp(ast->children[0]->opt.code_opt.lang_str, "c++", 3) == 0);
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block Language with Number");
    ast = kevlar_md_generate_ast("```f90\nprogram test\nend program\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    assert(ast->children[0]->opt.code_opt.lang_str_len == 3);
    assert(strncmp(ast->children[0]->opt.code_opt.lang_str, "f90", 3) == 0);
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block Uppercase Language");
    ast = kevlar_md_generate_ast("```PYTHON\ncode\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    // Language should be preserved as-is or normalized to lowercase
    assert(ast->children[0]->opt.code_opt.lang_str_len == 6);
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Complex Document with Code Blocks");
    ast = kevlar_md_generate_ast("# Code Examples\n\n"
                                 "Here's some Python:\n\n"
                                 "```python\ndef hello():\n    print('world')\n```\n\n"
                                 "And some JavaScript:\n\n"
                                 "```javascript\nconst hello = () => console.log('world');\n```\n\n"
                                 "## Summary\n\n"
                                 "Both examples print *hello world*.");

    test_check_count_and_type(ast, 7, MD_ROOT_NODE);

    // H1
    test_check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    assert(ast->children[0]->opt.h_opt.level == 1);
    test_match_text_node_text(ast->children[0]->children[0], "Code Examples");

    // Para
    test_check_count_and_type(ast->children[1], 1, MD_PARA_NODE);
    test_match_text_node_text(ast->children[1]->children[0], "Here's some Python:");

    // Code block - Python
    test_check_count_and_type(ast->children[2], 1, MD_CODE_BLOCK);
    assert(ast->children[2]->opt.code_opt.lang_str_len == 6);
    assert(strncmp(ast->children[2]->opt.code_opt.lang_str, "python", 6) == 0);
    test_match_text_node_text(ast->children[2]->children[0], "def hello():\n    print('world')");

    // Para
    test_check_count_and_type(ast->children[3], 1, MD_PARA_NODE);
    test_match_text_node_text(ast->children[3]->children[0], "And some JavaScript:");

    // Code block - JavaScript
    test_check_count_and_type(ast->children[4], 1, MD_CODE_BLOCK);
    assert(ast->children[4]->opt.code_opt.lang_str_len == 10);
    assert(strncmp(ast->children[4]->opt.code_opt.lang_str, "javascript", 10) == 0);
    test_match_text_node_text(ast->children[4]->children[0],
                              "const hello = () => console.log('world');");

    // H2 + Para with emphasis
    test_check_count_and_type(ast->children[5], 1, MD_HEADING_NODE);

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block with Windows Line Endings");
    ast = kevlar_md_generate_ast("```\r\nline1\r\nline2\r\n```");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    // Should handle \r\n properly
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Code Block Very Long");
    char long_code[5000];
    memset(long_code, 'x', 4990);
    long_code[4990] = '\0';
    char full_markdown[5100];
    snprintf(full_markdown, sizeof(full_markdown), "```\n%s\n```", long_code);
    ast = kevlar_md_generate_ast(full_markdown);
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_CODE_BLOCK);
    assert(ast->children[0]->children[0]->opt.text_opt.len == 4991);
    kevlar_md_free_ast(ast);
    /*************************************/

    puts("\n=== All code block tests completed ===\n");
}

void test_md_basic() {
    Md_Ast *ast = kevlar_md_generate_ast("Hello, World!");

    assert(ast->node_type == MD_ROOT_NODE && "AST must have a root node");

    kevlar_md_free_ast(ast);
}

void test_markdown() {
    puts("INFO: test_md_content()");
    test_md_content();
    puts("SUCCESS: test_md_content()");

#if 0
    puts("INFO: test_md_basic()");
    test_md_basic();
    puts("SUCCESS: test_md_basic()");

    puts("INFO: test_md_heading()");
    test_md_heading();
    puts("SUCCESS: test_md_heading()");

    puts("INFO: test_kevlar_md_find_next_occurrence()");
    test_kevlar_md_find_next_occurrence();
    puts("SUCCESS: test_kevlar_md_find_next_occurrence()");

    puts("INFO: test_md_inline_code(()");
    test_md_inline_code();
    puts("SUCCESS: test_md_inline_code(()");

    puts("INFO: test_md_links()");
    test_md_links();
    puts("SUCCESS: test_md_links()");

    puts("INFO: test_md_code_block()");
    test_md_code_block();
    puts("SUCCESS: test_md_code_block)");
#endif
};
