#include "kevlar_markdown.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

Md_Ast* kevlar_md_process_text_node(const char* source, size_t* pos, bool allow_line_break) {

    (void) allow_line_break;
    (void) source;
    (void) pos;

    return NULL;
}

Md_Ast* kevlar_md_process_heading_node(const char* source, size_t* pos) {
    Md_Ast *ast_node;
    if ((ast_node = malloc(sizeof(Md_Ast))) == NULL) {
        return NULL;
    }

    ast_node->node_type = MD_HEADING_NODE;

    size_t src_len = strlen(source);

    int level = 1; // we factor in that the current element is a '#'
    for (size_t i = *pos; i < src_len; ++i) {
        if (source[i] == '#') level++;

        if (source[i] == ' ') {
            if (level > 6) goto parsing_error;
            ast_node->opt.h_opt.level = level;

            (*pos)=i;

            if (i+1 > src_len) return ast_node;

            (*pos)++;

            Md_Ast* txt_node;
            if ((txt_node = kevlar_md_process_text_node(source, pos, false)) == NULL) return ast_node;

            if (!realloc(ast_node->children, sizeof(Md_Ast*)*ast_node->c_count+1)) goto parsing_error;

            ast_node->c_count++;

            ast_node->children[ast_node->c_count] = txt_node;

            return ast_node;

            break;
        }
    }

parsing_error: 
    free(ast_node);
    return NULL;

}

void kevlar_md_ast_child_append(Md_Ast *parent, Md_Ast *child) {
    (void)realloc(parent->children, sizeof(Md_Ast*)*parent->c_count+1);

    parent->c_count++;

    parent->children[parent->c_count+1] = child;
}

Md_Ast* kevlar_md_generate_ast(const char* source) {
    (void)source;
    Md_Ast *ast;
    if ((ast = malloc(sizeof(Md_Ast))) == NULL) {
        return NULL;
    }
        
    ast->node_type = MD_ROOT_NODE;
    ast->children = NULL;
    ast->c_count = 0;


    for (size_t i = 0; i <= strlen(source); ++i) {
        switch (source[i]) {
            case '#': {
                Md_Ast* h_node;
                if ((h_node = kevlar_md_process_heading_node(source, &i)) != NULL) 
                        kevlar_md_ast_child_append(ast, h_node);
                break;
            }

        }
    }

    return ast;
}

void kevlar_md_free_ast(Md_Ast* ast) {
    for (size_t i = 0; i < ast->c_count; i++) {
        Md_Ast* child = ast->children[i];

        if (child->node_type == MD_TEXT_NODE) {
            free(child->opt.text_opt.data);
        }

        kevlar_md_free_ast(child);
    }


    free(ast);
}
