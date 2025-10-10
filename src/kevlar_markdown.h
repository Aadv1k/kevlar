#ifndef _KEVLAR_MARKDOWN_H
#define _KEVLAR_MARKDOWN_H

#include <stdio.h>

typedef enum e_NodeType {
    MD_ROOT_NODE,

    MD_TEXT_NODE,
    MD_PAR_NODE,

    MD_HEADING_NODE,
} NodeType;

typedef struct Md_Text_Opt {
    char* data;
    size_t len;
} Md_Text_Opt;

typedef struct Md_H_Opt {
    unsigned char level;
} Md_H_Opt;

typedef struct Md_Ast {
    NodeType node_type;
    struct Md_Ast** children;
    size_t c_count;
    union {
        Md_Text_Opt text_opt;
        Md_H_Opt h_opt;
    } opt;
} Md_Ast;

Md_Ast* kevlar_md_generate_ast(const char* source);

void kevlar_md_free_ast(Md_Ast* ast);

#endif //  _KEVLAR_MARKDOWN_H
