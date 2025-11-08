#include "kevlar_markdown.h"
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
Md_Delem_Properties _kevlar_md_delim_properties(const char *src, size_t len, size_t pos,
                                                size_t run) {
    Md_Delem_Properties props = {0};

    bool has_preceeding_whitespace_or_punct =
        (int)(pos - 1) < 0 ? true : (isspace(src[pos - 1]) || ispunct((unsigned char)src[pos - 1]));

    bool is_left_flanking_run =
        pos + run < len && !isspace(src[pos + run]) &&
        (!ispunct((unsigned char)src[pos + run]) ||
         (ispunct((unsigned char)src[pos + run]) && has_preceeding_whitespace_or_punct));

    bool has_proceeding_whitespace_or_punct =
        (pos + run) > len ? true
                          : (isspace(src[pos + run]) || ispunct((unsigned char)src[pos + run]));

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
             (is_left_flanking_run && pos + run < len && ispunct((unsigned char)src[pos + run]))))
            props.closing = true;
    }

    return props;
}

typedef struct Delim {
    NodeType type;
    char variant;
    size_t pos;
    int run_offset;
    bool is_opening;
} Delim;

bool _kevlar_md_find_closing_delim(const char *src, size_t len, size_t cursor,
                                   Delim *closing_delim) {
    // TODO: find a good default for this
    Delim stack[100] = {};
    size_t stack_pos = 0;

    for (size_t i = cursor; i < len; ++i) {
        switch (src[i]) {
        case '*':
        case '_': {
            bool is_double = i + 1 < len && src[i + 1] == src[i];
            bool is_triple = is_double && i + 2 < len && src[i + 2] == src[i];

            int delim_run = (is_double && !is_triple) ? 2 : 1;
            NodeType n_type = (is_double && !is_triple) ? MD_STRONG_NODE : MD_EM_NODE;

            Md_Delem_Properties props = _kevlar_md_delim_properties(src, len, i, delim_run);

            if (is_triple && (props.closing || (!props.closing && !props.opening))) {
                delim_run = 2;
                n_type = MD_STRONG_NODE;
                props = _kevlar_md_delim_properties(src, len, i, delim_run);
            }

            if ((props.closing || (!props.closing && !props.opening)) && stack_pos > 0 &&
                stack[stack_pos - 1].is_opening && stack[stack_pos - 1].variant == src[i] &&
                stack[stack_pos - 1].type == n_type) {
                // TODO: the below line is a bit sus
                if (stack[stack_pos - 1].pos == cursor && ((stack_pos - 1) == 0)) {

                    closing_delim->is_opening = false;
                    closing_delim->pos = i + (delim_run - 1);
                    closing_delim->run_offset = delim_run;
                    closing_delim->variant = stack[stack_pos - 1].variant;
                    closing_delim->type = n_type;

                    return true;
                }
                stack_pos--;
            } else {
                stack[stack_pos].is_opening = true;
                stack[stack_pos].variant = src[i];
                stack[stack_pos].pos = i;
                stack[stack_pos].run_offset = delim_run;
                stack[stack_pos].type = n_type;
                stack_pos++;
            }

            i += delim_run - 1;
            // }
        }
        }
    }

    return false;
}

int kevlar_md_process_text_node(const char *src, size_t len, size_t *cursor, Md_Ast *parent,
                                unsigned int allowed_line_ends) {
    char text_buffer[MD_MAX_TEXT_BUFFER];
    size_t text_buffer_pos = 0;

    for (size_t i = *cursor; i < len; ++i) {
        if (src[i] == '*' || src[i] == '_') {
            Delim closing_delim = {0};
            if (_kevlar_md_find_closing_delim(src, len, i, &closing_delim)) {
                Md_Ast *em_or_strong_node = malloc(sizeof(Md_Ast));
                em_or_strong_node->node_type = closing_delim.type;

                // TODO: why do we need to handle it like so?
                size_t buf_len = closing_delim.pos - i -
                                 (closing_delim.run_offset > 1 ? closing_delim.run_offset + 1
                                                               : closing_delim.run_offset);

                char *buffer = malloc(sizeof(char) * buf_len);
                strncpy(buffer, &src[i + closing_delim.run_offset], buf_len);

                size_t sub_sub_cur = 0;
                kevlar_md_process_text_node(buffer, buf_len, &sub_sub_cur, em_or_strong_node,
                                            MD_DOUBLE_LINE_BREAK);
                if (text_buffer_pos != 0) {
                    Md_Ast *txt_node =
                        create_text_node_from_buffer_if_valid(text_buffer, text_buffer_pos);
                    kevlar_md_ast_child_append(parent, txt_node);
                    text_buffer_pos = 0;
                }
                kevlar_md_ast_child_append(parent, em_or_strong_node);
                free(buffer);

                *cursor = closing_delim.pos;
                i = *cursor;
                continue;
            }
        } else if (src[i] == '\n') {
            Md_Line_End_Type line_end = get_line_end_type(src, i);

            bool should_break =
                (line_end == MD_DOUBLE_LINE_BREAK && !(allowed_line_ends & MD_DOUBLE_LINE_BREAK)) ||
                (line_end == MD_SINGLE_LINE_BREAK && !(allowed_line_ends & MD_SINGLE_LINE_BREAK)) ||
                line_end == MD_EOF;

            if (should_break) {
                *cursor = line_end == MD_DOUBLE_LINE_BREAK ? i + 1 : i;
                i = *cursor;

                Md_Ast *txt_node =
                    create_text_node_from_buffer_if_valid(text_buffer, text_buffer_pos);

                if (txt_node != NULL) {
                    kevlar_md_ast_child_append(parent, txt_node);
                }

                // TODO: remove this down the line
                assert(
                    strlen(txt_node->opt.text_opt.data) == text_buffer_pos &&
                    "If this fails, it means some memory corruption has happened within the text");

                return 0;
            }
        }

        if (src[i] == '\\' && i + 1 < len && (SPECIAL_CHAR_SET[(unsigned char)src[i + 1]])) {
            text_buffer[text_buffer_pos] = src[i + 1];
            text_buffer_pos++;

            i++;
            (*cursor) += 2;
            continue;
        }

        text_buffer[text_buffer_pos] = src[i];
        text_buffer_pos++;
        (*cursor)++;
    }

    if (text_buffer_pos != 0 &&
        utl_lstrip_offset(text_buffer, text_buffer_pos) != text_buffer_pos) {
        Md_Ast *txt_node = create_text_node_from_buffer_if_valid(text_buffer, text_buffer_pos);
        kevlar_md_ast_child_append(parent, txt_node);
    }
    return 0;
}

Md_Ast *kevlar_md_process_heading_node(const char *source, size_t *pos) {
    Md_Ast *ast_node;
    if ((ast_node = malloc(sizeof(Md_Ast))) == NULL) {
        return NULL;
    }

    ast_node->node_type = MD_HEADING_NODE;

    size_t src_len = strlen(source);

    // ### Hell world ## how are you doing
    //    ^
    // ###
    unsigned char level = 0;
    for (size_t i = *pos; i <= src_len; ++i) {
        if (source[i] == '#')
            level++;

        if (isspace(source[i]) != 0) {
            if (level > 6)
                goto parsing_error;
            ast_node->opt.h_opt.level = level;

            (*pos) = i + 1; // accounting for the space as well

            if (i >= src_len)
                return ast_node;

            int ret = kevlar_md_process_text_node(source, src_len, pos, ast_node, MD_DOUBLE_LINE_BREAK);
            if (ret != 0)
                goto parsing_error;

            if (ast_node->c_count > 0 && ast_node->children[0]->node_type == MD_TEXT_NODE) {
                Md_Ast *txt_node = ast_node->children[0];

                size_t left_offset =
                    utl_lstrip_offset(txt_node->opt.text_opt.data, txt_node->opt.text_opt.len);
                size_t new_len = txt_node->opt.text_opt.len - left_offset;

                if (left_offset) {
                    char *new_data = malloc(sizeof(char) * new_len);
                    strncpy(new_data, txt_node->opt.text_opt.data + left_offset, new_len);
                    free(txt_node->opt.text_opt.data);

                    txt_node->opt.text_opt.data = new_data;
                    txt_node->opt.text_opt.len = new_len;
                }
            }

            return ast_node;
        }
    }

    return NULL;

parsing_error:
    free(ast_node);
    return NULL;
}

Md_Ast *kevlar_md_generate_ast(const char *source) {
    SPECIAL_CHAR_SET['*'] = 1;
    SPECIAL_CHAR_SET['_'] = 1;
    SPECIAL_CHAR_SET['~'] = 1;
    SPECIAL_CHAR_SET['`'] = 1;
    SPECIAL_CHAR_SET['#'] = 1;
    SPECIAL_CHAR_SET['\\'] = 1;
    SPECIAL_CHAR_SET[']'] = 1;
    SPECIAL_CHAR_SET['['] = 1;
    SPECIAL_CHAR_SET[')'] = 1;
    SPECIAL_CHAR_SET['('] = 1;

    Md_Ast *ast;
    if ((ast = malloc(sizeof(Md_Ast))) == NULL) {
        return NULL;
    }

    ast->node_type = MD_ROOT_NODE;

    size_t src_len = strlen(source);

    for (size_t i = 0; i < src_len; ++i) {
        if (source[i] == '#') {
            Md_Ast *h_node;
            if ((h_node = kevlar_md_process_heading_node(source, &i)) != NULL) {
                kevlar_md_ast_child_append(ast, h_node);
                continue;
            }
        }

        Md_Ast *para_node = malloc(sizeof(Md_Ast));
        para_node->node_type = MD_PARA_NODE;

        int ret =
            kevlar_md_process_text_node(source, src_len, &i, para_node, MD_DOUBLE_LINE_BREAK);

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
