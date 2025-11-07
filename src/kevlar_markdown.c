#include "kevlar_markdown.h"
#include "kevlar_build.h"
#include "kevlar_errors.h"
#include "utils.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define MD_MAX_TEXT_BUFFER 50000 // 50 KiB, should be enough for most?
#define SPECIAL_CHAR_SET_LEN 128

bool SPECIAL_CHAR_SET[SPECIAL_CHAR_SET_LEN] = {0};

Md_Line_End_Type get_line_end_type(const char *source, size_t pos) {
    size_t len = strlen(source);

    if (pos + 1 > len)
        return MD_EOF;

    if (source[pos] == '\n' && source[pos + 1] == '\n')
        return MD_DOUBLE_LINE_BREAK;

    return MD_SINGLE_LINE_BREAK;
}

void kevlar_md_ast_child_append(Md_Ast *parent, Md_Ast *child) {
    assert(parent != NULL);

    Md_Ast **a;
    if ((a = realloc(parent->children, sizeof(Md_Ast *) * parent->c_count + 1)) == NULL) {
        kevlar_err("Could not re-allocate memory!");
    }

    parent->children = a;
    parent->children[parent->c_count] = child;
    parent->c_count++;
}

Md_Ast *create_text_node_from_buffer_if_valid(char *buffer, size_t buffer_len) {
    if (buffer_len == 0)
        return NULL;

    Md_Ast *txt_node = malloc(sizeof(Md_Ast));

    txt_node->node_type = MD_TEXT_NODE;
    txt_node->opt.text_opt.len = buffer_len + 1;

    txt_node->opt.text_opt.data = malloc(sizeof(char) * buffer_len + 1);
    strncpy(txt_node->opt.text_opt.data, buffer, buffer_len);
    txt_node->opt.text_opt.data[buffer_len] = '\0';

    return txt_node;
}

typedef struct Md_Delem_Properties {
    bool opening;
    bool closing;
} Md_Delem_Properties;

// REF: https://spec.commonmark.org/0.31.2/#left-flanking-delimiter-run
Md_Delem_Properties _kevlar_md_em_delim_properties(const char *src, size_t len, size_t pos) {
    Md_Delem_Properties props = {0};

    bool has_preceeding_whitespace_or_punct =
        (int)(pos - 1) < 0 ? true : (isspace(src[pos - 1]) || ispunct((unsigned char)src[pos - 1]));

    bool is_left_flanking_run =
        pos + 1 < len && !isspace(src[pos + 1]) &&
        (!ispunct((unsigned char)src[pos + 1]) ||
         (ispunct((unsigned char)src[pos + 1]) && has_preceeding_whitespace_or_punct));

    bool has_proceeding_whitespace_or_punct =
        (pos + 1) > len ? true : (isspace(src[pos + 1]) || ispunct((unsigned char)src[pos + 1]));

    bool is_right_flanking_run =
        ((int)pos - 1 >= 0 && !isspace(src[pos - 1]) &&
         (!ispunct((unsigned char)src[pos - 1]) ||
          (ispunct((unsigned char)src[pos - 1]) && has_proceeding_whitespace_or_punct)));

    if (src[pos] == '*') {
        if (is_left_flanking_run)
            props.opening = true;
        if (is_right_flanking_run)
            props.closing = true;
    } else if (src[pos] == '_') {
        if (is_left_flanking_run &&
            (!is_right_flanking_run ||
             (is_right_flanking_run && (int)pos - 1 > 0 && ispunct((unsigned char)src[pos - 1]))))
            props.opening = true;

        if (is_right_flanking_run &&
            (!is_left_flanking_run ||
             (is_left_flanking_run && pos + 1 < len && ispunct((unsigned char)src[pos + 1]))))
            props.closing = true;
    }

    return props;
}

typedef struct Delim {
    NodeType type;
    char variant;
    size_t pos;
    bool is_opening;
} Delim;

bool _kevlar_md_find_closing_em_or_strong(const char *src, size_t len, size_t cursor,
                                          size_t *index) {
    Delim stack[100] = {};
    size_t stack_pos = 0;

    for (size_t i = cursor; i < len; ++i) {
        if (src[i] == '*' || src[i] == '_') {
            Md_Delem_Properties props = _kevlar_md_em_delim_properties(src, len, i);
            if (props.closing && stack_pos > 0 && stack[stack_pos - 1].is_opening &&
                stack[stack_pos - 1].variant == src[i] && stack[stack_pos - 1].type == MD_EM_NODE) {
                // TODO: the below line is a bit sus
                if (stack[stack_pos - 1].pos == cursor && ((stack_pos - 1) == 0)) {
                    *index = i;
                    return true;
                }
                stack_pos--;
            } else {
                stack[stack_pos].is_opening = true;
                stack[stack_pos].variant = src[i];
                stack[stack_pos].pos = i;
                stack[stack_pos].type = MD_EM_NODE;
                stack_pos++;
            }
        }
    }

    return false;
}

int kevlar_md_process_text_node(const char *src, size_t len, size_t *cursor, Md_Ast *parent,
                                Delim_Pairs *pairs, Md_Line_End_Type line_end_type) {
    (void)line_end_type;
    char text_buffer[MD_MAX_TEXT_BUFFER];
    size_t text_buffer_pos = 0;

    (void)pairs;

    for (size_t i = *cursor; i < len; ++i) {
        if (src[i] == '*' || src[i] == '_') {
            size_t sub_cur;
            if (_kevlar_md_find_closing_em_or_strong(src, len, i, &sub_cur)) {
                Md_Ast *em_node = malloc(sizeof(Md_Ast));
                em_node->node_type = MD_EM_NODE;

                size_t buf_len = sub_cur - i - 1;
                char *buffer = malloc(sizeof(char) * buf_len);
                strncpy(buffer, &src[i + 1], buf_len);

                size_t sub_sub_cur = 0;
                kevlar_md_process_text_node(buffer, buf_len, &sub_sub_cur, em_node, NULL,
                                            MD_DOUBLE_LINE_BREAK);
                if (text_buffer_pos != 0) {
                    Md_Ast *txt_node =
                        create_text_node_from_buffer_if_valid(text_buffer, text_buffer_pos);
                    kevlar_md_ast_child_append(parent, txt_node);
                    text_buffer_pos = 0;
                }
                kevlar_md_ast_child_append(parent, em_node);
                free(buffer);
                *cursor = sub_cur;
                i = *cursor;
                continue;
            }
        }

        text_buffer[text_buffer_pos] = src[i];
        text_buffer_pos++;
        (*cursor)++;
    }

    if (text_buffer_pos != 0) {
        Md_Ast *txt_node = create_text_node_from_buffer_if_valid(text_buffer, text_buffer_pos);
        kevlar_md_ast_child_append(parent, txt_node);
    }

    return 0;
}

Md_Ast *kevlar_md_generate_ast(const char *source) {
    Md_Ast *ast;
    if ((ast = malloc(sizeof(Md_Ast))) == NULL) {
        return NULL;
    }

    ast->node_type = MD_ROOT_NODE;

    size_t src_len = strlen(source);

    for (size_t i = 0; i < src_len; ++i) {
        Md_Ast *para_node = malloc(sizeof(Md_Ast));
        para_node->node_type = MD_PARA_NODE;

        int ret =
            kevlar_md_process_text_node(source, src_len, &i, para_node, NULL, MD_DOUBLE_LINE_BREAK);

        if (ret != 0) {
            free(para_node);
            kevlar_err("Could not process text node at position %zu\n", i);
        }

        kevlar_md_ast_child_append(ast, para_node);
    }

    return ast;
}

void kevlar_md_free_ast(Md_Ast *ast) {
    for (size_t i = 0; i < ast->c_count; ++i) {
        if (ast->children[i]->node_type == MD_TEXT_NODE) {
            free(ast->children[i]->opt.text_opt.data);
        }

        if (ast->children[i]->node_type == MD_LINK_NODE) {
            free(ast->children[i]->opt.link_opt.href_str);
        }

        kevlar_md_free_ast(ast->children[i]);
    }

    free(ast);
}
