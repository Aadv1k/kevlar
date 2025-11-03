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
    MD_SINGLE_LINE_BREAK,
    MD_DOUBLE_LINE_BREAK,
    MD_EOF,
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
    char* lang_str;
    size_t lang_str_len;
} Md_Code_Opt;

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

size_t kevlar_md_find_next_occurrence(const char *data, size_t len, size_t start, const char *tag,
                                      size_t *index, bool respect_line_break, bool respect_double_line_break);

Md_Ast *kevlar_md_process_text_node(const char *source, size_t *pos, bool allow_line_breaks);

#endif //  _KEVLAR_MARKDOWN_H
