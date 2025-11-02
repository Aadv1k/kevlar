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

typedef struct Md_Ast {
    NodeType node_type;
    struct Md_Ast **children;
    size_t c_count;
    union {
        Md_Text_Opt text_opt;
        Md_H_Opt h_opt;
    } opt;
} Md_Ast;

Md_Ast *kevlar_md_generate_ast(const char *source);

void kevlar_md_free_ast(Md_Ast *ast);

size_t kevlar_md_find_next_occurrence(const char *data, size_t len, size_t start, const char *tag,
                                      size_t *index, bool respect_line_break);

Md_Ast *kevlar_md_process_text_node(const char *source, size_t *pos, bool allow_line_breaks);

#endif //  _KEVLAR_MARKDOWN_H
