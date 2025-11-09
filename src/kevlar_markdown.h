#ifndef _KEVLAR_MARKDOWN_H
#define _KEVLAR_MARKDOWN_H

#include <stdbool.h>
#include <stdio.h>

typedef enum e_NodeType {
    MD_ROOT_NODE = 0,
    MD_PARA_NODE,
    MD_EM_NODE,
    MD_STRONG_NODE,
    MD_STRONG_EM_NODE, // hacky solution
    MD_DEL_NODE,
    MD_TEXT_NODE,
    MD_INLINE_CODE_BLOCK,
    MD_HEADING_NODE,
    MD_LINK_NODE,
    MD_IMG_LINK,

    MD_CODE_BLOCK,

    MD_BLOCKQUOTE_NODE,

    MD_TABLE_NODE,
    MD_TABLE_ROW,
    MD_TABLE_CELL,

    MD_LIST_NODE,
    MD_LIST_ITEM_NODE,
} NodeType;

typedef enum Md_Line_End_Type {
    MD_SINGLE_LINE_BREAK = 1 << 0,
    MD_DOUBLE_LINE_BREAK = 1 << 1,
    MD_EOF = 1 << 2,
} Md_Line_End_Type;

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

typedef struct Md_Code_Opt {
    char *lang_str;
    size_t lang_str_len;
} Md_Code_Opt;

typedef enum Md_Delim_Run_Type {
    MD_DELEM_RIGHT_FLANK,
    MD_DELEM_LEFT_FLANK,
    MD_DELEM_UNDEFINED
} Md_Delem_Run_Type;

struct Delim_Pair {
    NodeType type;
    char variant;
    size_t start;
    size_t end;
};

typedef struct Delim_Pairs {
    struct Delim_Pair pairs[100];
    size_t count;
} Delim_Pairs;

typedef struct Md_Ast {
    NodeType node_type;
    struct Md_Ast **children;
    size_t c_count;
    union {
        Md_Text_Opt text_opt;
        Md_H_Opt h_opt;
        Md_Link_Opt link_opt;
        Md_List_Opt list_opt;
        Md_Code_Opt code_opt;
    } opt;
} Md_Ast;

Md_Ast *kevlar_md_generate_ast(const char *source);

void kevlar_md_free_ast(Md_Ast *ast);

int kevlar_md_process_text_node(const char *src, size_t len, size_t *cursor, Md_Ast *parent, unsigned int allowed_line_ends);

Md_Ast *kevlar_md_process_code_block_node(const char *src, size_t len, size_t *pos);

#endif //  _KEVLAR_MARKDOWN_H
