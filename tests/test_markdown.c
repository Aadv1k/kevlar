#include "../src/kevlar_markdown.h"

#include <stdio.h>
#include <assert.h>

void test_md_heading() {
    Md_Ast* ast;

    ast = kevlar_md_generate_ast("# Foo");

    assert(ast->c_count == 1 && "Single heading child");
    assert(ast->children[ast->c_count]->node_type == MD_HEADING_NODE && "Child must be heading node ");

    kevlar_md_free_ast(ast);
}

void test_md_basic() {
    Md_Ast* ast = kevlar_md_generate_ast("Hello, World!");

    assert(ast->node_type == MD_ROOT_NODE && "AST must have a root node");

    kevlar_md_free_ast(ast);
}

void test_markdown() {
    test_md_basic();
    test_md_heading();

    printf("SUCCESS: test_markdown()\n");
};
