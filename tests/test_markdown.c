#include "../src/kevlar_markdown.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

void test_md_heading() {
    Md_Ast* ast;

    /*************************************/
    puts("Test A");
    ast = kevlar_md_generate_ast("# Foo");

    assert(ast->c_count == 1 && "Root has single child");
    assert(ast->children[ast->c_count-1]->node_type == MD_HEADING_NODE && "Child must be heading node");

    Md_Ast* h_node = ast->children[ast->c_count-1];

    assert(h_node->c_count == 1 && "Heading must have a child");
    assert(h_node->children[0]->node_type == MD_TEXT_NODE);
    assert(strcmp(h_node->children[0]->opt.text_opt.data, "Foo") == 0);

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test B");
    ast = kevlar_md_generate_ast("###");

    assert(!(ast->c_count > 1));
    assert(ast->children[ast->c_count-1]->node_type == MD_TEXT_NODE);

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test C");
    ast = kevlar_md_generate_ast("##### A");

    assert(ast->c_count == 1 && "Single heading child");
    assert(ast->children[ast->c_count-1]->node_type == MD_HEADING_NODE && "Child must be heading node");

    assert(ast->children[0]->c_count == 1 && "Heading must have a child");
    assert(ast->children[0]->children[0]->node_type == MD_TEXT_NODE);
    assert(strcmp(ast->children[0]->children[0]->opt.text_opt.data, "A") == 0);

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test D");
    ast = kevlar_md_generate_ast("########## Should NOT be a heading");

    assert(ast->c_count == 1);
    assert(ast->children[0]->node_type == MD_TEXT_NODE);
    assert(strcmp(ast->children[0]->opt.text_opt.data, "########## Should NOT be a heading") == 0);

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test E");
    ast = kevlar_md_generate_ast("## ");

    assert(ast->c_count == 1);
    assert(ast->children[0]->node_type == MD_HEADING_NODE);
    assert(ast->children[0]->c_count == 0);

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test F");
    ast = kevlar_md_generate_ast("### ## ### # ## # # # # ## ##### ");

    assert(ast->c_count == 1);
    assert(ast->children[0]->node_type == MD_HEADING_NODE);
    assert(ast->children[0]->c_count == 1);
    assert(ast->children[0]->children[0]->node_type == MD_TEXT_NODE);
    assert(strcmp(ast->children[0]->children[0]->opt.text_opt.data, "## ### # ## # # # # ## ##### ") == 0);

    kevlar_md_free_ast(ast);
    /*************************************/

}

void test_md_basic() {
    Md_Ast* ast = kevlar_md_generate_ast("Hello, World!");

    assert(ast->node_type == MD_ROOT_NODE && "AST must have a root node");

    kevlar_md_free_ast(ast);
}

void test_markdown() {
    puts("INFO: test_md_basic()");
    test_md_basic();
    puts("SUCCESS: test_md_basic()");

    puts("INFO: test_md_heading()");
    test_md_heading();
    puts("SUCCESS: test_md_heading()");
};
