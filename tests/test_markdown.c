#include "../src/kevlar_markdown.h"
#include "../src/utils.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

void test_check_count_and_type(Md_Ast* ast, size_t c_count, NodeType typ) {
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

void test_match_text_node_text(Md_Ast* txt_node, const char* dest) {
    assert(txt_node->node_type == MD_TEXT_NODE);

    if (strcmp(txt_node->opt.text_opt.data, dest) != 0) {
        printf("\tWANTED: %s\n", dest);
        printf("\tGOT: %s\n", txt_node->opt.text_opt.data);
        assert(0 && "txt_node content did not match data");
    }
}

void test_md_heading() {
    Md_Ast* ast;
    
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
}


void test_md_content() {
    Md_Ast* ast;

    /*************************************/
    puts("Test A");
    ast = kevlar_md_generate_ast("Hello *World*");

    /*
     * - Para
     * -- text
     * -- em
     * --- text
     */


    utl_visualize_ast(ast, 0);

    test_check_count_and_type(ast->children[0], 2, MD_PARA_NODE);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);

    test_check_count_and_type(ast->children[0]->children[1], 1, MD_EM_NODE);
        test_check_count_and_type(ast->children[0]->children[1]->children[0], 0, MD_TEXT_NODE);

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test B");
    ast = kevlar_md_generate_ast("The _quick_ *brown* **fox** ***jumps*** ~~over~~ the lazy dog");

    test_check_count_and_type(ast->children[0], 1, MD_PARA_NODE);

    test_check_count_and_type(ast->children[0]->children[0], 0, MD_TEXT_NODE);

    test_check_count_and_type(ast->children[0]->children[1], 1, MD_EM_NODE);
        test_check_count_and_type(ast->children[0]->children[1]->children[0], 0, MD_TEXT_NODE);

    test_check_count_and_type(ast->children[0]->children[2], 1, MD_EM_NODE);
        test_check_count_and_type(ast->children[0]->children[2]->children[0], 0, MD_TEXT_NODE);

    test_check_count_and_type(ast->children[0]->children[3], 1, MD_STRONG_NODE);
        test_check_count_and_type(ast->children[0]->children[3]->children[0], 0, MD_TEXT_NODE);

    test_check_count_and_type(ast->children[0]->children[4], 1, MD_EM_NODE);
        test_check_count_and_type(ast->children[0]->children[4]->children[0], 1, MD_STRONG_NODE);
            test_check_count_and_type(ast->children[0]->children[4]->children[0]->children[0], 0, MD_TEXT_NODE);

    test_check_count_and_type(ast->children[0]->children[5], 1, MD_DEL_NODE);
        test_check_count_and_type(ast->children[0]->children[5]->children[0], 0, MD_TEXT_NODE);

    test_check_count_and_type(ast->children[0]->children[6], 0, MD_TEXT_NODE);

    kevlar_md_free_ast(ast);
    /*************************************/

}


void test_md_basic() {
    Md_Ast* ast = kevlar_md_generate_ast("Hello, World!");

    assert(ast->node_type == MD_ROOT_NODE && "AST must have a root node");

    kevlar_md_free_ast(ast);
}

void test_markdown() {

#if 0
    puts("INFO: test_md_basic()");
    test_md_basic();
    puts("SUCCESS: test_md_basic()");
#endif

    puts("INFO: test_md_heading()");
    test_md_heading();
    puts("SUCCESS: test_md_heading()");

    puts("INFO: test_md_content()");
    test_md_content();
    puts("SUCCESS: test_md_content()");
};
