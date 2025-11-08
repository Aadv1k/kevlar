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
    puts("Test F");
    ast = kevlar_md_generate_ast("# ये हाथ मुझे दे दे ठाकुर!");
    test_check_count_and_type(ast, 1, MD_ROOT_NODE);
    test_check_count_and_type(ast->children[0], 1, MD_HEADING_NODE);
    assert(ast->children[0]->opt.h_opt.level == 1);
    test_match_text_node_text(ast->children[0]->children[0], "ये हाथ मुझे दे दे ठाकुर!");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test G");
    ast = kevlar_md_generate_ast("# H1\n## H2\n### H3\n#### H4\n##### H5\n###### H6");
    test_check_count_and_type(ast, 6, MD_ROOT_NODE);
    for (int i = 0; i < 6; i++) {
        test_check_count_and_type(ast->children[i], 1, MD_HEADING_NODE);
        assert(ast->children[i]->opt.h_opt.level == i + 1);
    }
    kevlar_md_free_ast(ast);
    /*************************************/
}

void test_md_emphasis_and_strong() {
    Md_Ast *ast;

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
    test_check_count_and_type(ast->children[0]->children[0]->children[0]->children[0], 0,
                              MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0]->children[0],
                              "Hello World");

    kevlar_md_free_ast(ast);
    /*************************************/

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
    puts("Test E");
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
    puts("Test F");
    ast = kevlar_md_generate_ast("This has *unclosed emphasis");
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "This has *unclosed emphasis");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test G");
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
    puts("Test H");
    ast = kevlar_md_generate_ast("***I got an appointment with _Dr. Monster_***");

    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 2, MD_STRONG_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0]->children[0], 0,
                              MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0]->children[0],
                              "I got an appointment with ");

    test_check_count_and_type(ast->children[0]->children[0]->children[0]->children[1], 1,
                              MD_EM_NODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0]->children[1]->children[0],
                              0, MD_TEXT_NODE);
    test_match_text_node_text(ast->children[0]->children[0]->children[0]->children[1]->children[0],
                              "Dr. Monster");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    /*************************************/
    puts("Test I");
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
    puts("Test J");
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
    puts("Test K");
    ast = kevlar_md_generate_ast("\\*not emphasis\\* but *this is*");
    test_check_count_and_type(ast->children[0], 2, MD_PARA_NODE);
    test_match_text_node_text(ast->children[0]->children[0], "*not emphasis* but ");
    test_check_count_and_type(ast->children[0]->children[1], 1, MD_EM_NODE);
    test_match_text_node_text(ast->children[0]->children[1]->children[0], "this is");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test L");
    ast = kevlar_md_generate_ast("     \t\t\t     ");
    test_check_count_and_type(ast->children[0], 0, MD_PARA_NODE);
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test M");
    char long_text[10000];
    memset(long_text, 'a', 9999);
    long_text[9999] = '\0';
    ast = kevlar_md_generate_ast(long_text);
    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);
    kevlar_md_free_ast(ast);
    /*************************************/
}

void test_md_basic() {
    Md_Ast *ast = kevlar_md_generate_ast("Hello, World!");

    assert(ast->node_type == MD_ROOT_NODE && "AST must have a root node");

    kevlar_md_free_ast(ast);
}

void test_markdown() {
    puts("INFO: test_md_basic()");
    test_md_basic();
    puts("SUCCESS: test_md_basic()");

    puts("INFO: test_md_emphasis_and_strong()");
    test_md_emphasis_and_strong();
    puts("SUCCESS: test_md_emphasis_and_strong()");

    puts("INFO: test_md_heading()");
    test_md_heading();
    puts("SUCCESS: test_md_heading()");
};
