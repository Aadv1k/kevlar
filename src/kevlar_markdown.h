#ifndef _KEVLAR_MARKDOWN_H
#define _KEVLAR_MARKDOWN_H

#include <stdbool.h>
#include <stdio.h>

typedef enum Md_Node_Type {
    MD_NODE_ROOT = 0,
    MD_NODE_PARAGRAPH,
    MD_NODE_EMPH,
    MD_NODE_STRONG,
    MD_NODE_STRIKETHRU,
    MD_NODE_TEXT,
    MD_NODE_INLINE_CODE,
    MD_NODE_HEADING,
    MD_NODE_LINK,
    MD_NODE_CODE_BLOCK,

    MD_NODE_LIST,
    MD_NODE_LIST_ITEM,
} Md_Node_Type;

typedef enum Md_Line_End_Type {
    MD_SINGLE_LINE_BREAK = 1 << 0,
    MD_DOUBLE_LINE_BREAK = 1 << 1,
    MD_EOF = 1 << 2,
} Md_Line_End_Type;

typedef struct Md_Delem_Properties {
    bool opening;
    bool closing;
} Md_Delem_Properties;

typedef struct Md_Text_Opt {
    char *data;
    size_t len;
} Md_Text_Opt;

typedef struct Md_H_Opt {
    unsigned char level;
} Md_H_Opt;

// The actual link label content is parsed as proper markdown text and added as children
typedef struct Md_Link_Opt {
    char *href_str;
    size_t href_len;
} Md_Link_Opt;

typedef struct Md_List_Opt {
    bool ordered;
    size_t ordered_list_start_num; // starting number of the ordered list
} Md_List_Opt;

typedef struct Md_Code_Block_Opt {
    char *lang_str;
    size_t lang_str_len;
} Md_Code_Block_Opt;

typedef struct Md_Ast {
    Md_Node_Type node_type;
    struct Md_Ast **children;
    size_t c_count;
    union {
        Md_Text_Opt text_opt;
        Md_H_Opt h_opt;
        Md_Link_Opt link_opt;
        Md_List_Opt list_opt;
        Md_Code_Block_Opt code_block_opt;
    } opt;
} Md_Ast;

Md_Ast *kevlar_md_generate_ast(const char *source);

void kevlar_md_free_ast(Md_Ast *ast);

int kevlar_md_process_text_node(const char *src, size_t len, size_t *cursor, Md_Ast *parent,
                                unsigned int allowed_line_ends);

Md_Ast *kevlar_md_process_code_block_node(const char *src, size_t len, size_t *pos);

#endif //  _KEVLAR_MARKDOWN_H
