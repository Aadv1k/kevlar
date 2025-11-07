#include "kevlar_markdown.h"
#include "kevlar_errors.h"
#include "utils.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

Md_Ast *create_text_node_from_buffer(char *buffer, size_t buffer_len) {
    Md_Ast *txt_node = malloc(sizeof(Md_Ast));

    txt_node->node_type = MD_TEXT_NODE;
    txt_node->opt.text_opt.len = buffer_len + 1;

    txt_node->opt.text_opt.data = malloc(sizeof(char) * buffer_len + 1);
    strncpy(txt_node->opt.text_opt.data, buffer, buffer_len);
    txt_node->opt.text_opt.data[buffer_len] = '\0';

    return txt_node;
}

size_t kevlar_md_find_next_occurrence(const char *data, size_t len, size_t start, const char *tag,
                                      size_t *index, bool respect_line_break, bool respect_double_line_break) {
    size_t tag_len = strlen(tag);

    for (size_t i = start; i < len; ++i) {
        size_t matches = 0;

        if (data[i] == '\n') {
            Md_Line_End_Type line_end = get_line_end_type(data, i);
            if ((respect_double_line_break && line_end == MD_DOUBLE_LINE_BREAK) ||
                (respect_line_break && line_end == MD_SINGLE_LINE_BREAK))
                return false;
        }

        if (data[i] == '\\' && i + 1 < len && SPECIAL_CHAR_SET[(unsigned char)data[i + 1]]) {
            i += 1;
            continue;
        }

        for (size_t j = 0; j < tag_len; ++j) {
            if (data[i + j] == tag[j])
                matches++;
        }

        if (matches == tag_len) {
            *index = i;
            return true;
        }
    }

    return false;
}

// NOTE: This approach has a fundamental limitation where same nested types, ie *foo *bar* baz* will
// not work. This is because the function will look ahead to the immediate *, and the match will
// look like this [foo ][bar][ baz] however, realistically this situation is rare. Getting around to
// this isn't hard but would require a rewrite of this function with some kind of stack
Md_Ast *kevlar_md_process_pair(const char *data, size_t len, const char *suffix, size_t *pos,
                               NodeType type, bool respect_line_break, bool formatting_enabled, bool respect_double_line_break) {

    size_t suffix_len = strlen(suffix), closing_pos;

    bool match_found =
        kevlar_md_find_next_occurrence(data, len, *pos, suffix, &closing_pos, respect_line_break, respect_double_line_break);

    if (!match_found)
        return NULL;

    /*
     * <tag>Foo Bar Baz</tag>
     *                 ^- closing_pos is here
     * To get the length we subtract the starting pos, and 1
     */
    size_t content_len = closing_pos - *pos;

    if (content_len == 0)
        return NULL;

    // TODO: this malloc is redundant, here we can directly use lenghts and pointers
    char *content_buffer = malloc(sizeof(char) * content_len + 1);
    size_t content_buffer_pos = 0;

    strncpy(content_buffer, &data[*pos], content_len);
    content_buffer[content_len] = '\0';

    if (!formatting_enabled) {
        Md_Ast *raw_node = malloc(sizeof(Md_Ast));
        raw_node->node_type = type;
        Md_Ast *txt_node = create_text_node_from_buffer(content_buffer, content_len);

        *pos = closing_pos + suffix_len - 1;
        kevlar_md_ast_child_append(raw_node, txt_node);

        free(content_buffer);

        return raw_node;
    }

    Md_Ast *root_txt_node =
        kevlar_md_process_text_node(content_buffer, &content_buffer_pos, !respect_line_break);

    if (!root_txt_node) {
        free(content_buffer);
        return NULL;
    }

    root_txt_node->node_type = type;
    // </foo>
    // ^----^-- closing_pos + suffix_len - 1 for adjustment
    *pos = closing_pos + suffix_len - 1;

    free(content_buffer);

    return root_txt_node;
}

bool _md_handle_escaping(char **buffer, size_t *buffer_len) {
    char tmp_buffer[MD_MAX_TEXT_BUFFER];
    size_t tmp_buffer_pos = 0;

    for (size_t i = 0; i < *buffer_len; ++i) {
        if ((*buffer)[i] == '\\' && i + 1 < *buffer_len &&
            SPECIAL_CHAR_SET[(unsigned char)(*buffer)[i + 1]]) {
            continue;
        }

        tmp_buffer[tmp_buffer_pos] = (*buffer)[i];
        tmp_buffer_pos++;
    }

    tmp_buffer[tmp_buffer_pos] = '\0';

    char *new_buffer = strndup(tmp_buffer, tmp_buffer_pos);
    if (!new_buffer) {
        *buffer = NULL;
        return false;
    }
    free(*buffer);
    *buffer = new_buffer;
    *buffer_len = tmp_buffer_pos;

    return true;
}

Md_Ast *kevlar_md_process_text_node(const char *source, size_t *pos, bool allow_line_breaks) {
    Md_Ast *ast_node;
    if ((ast_node = malloc(sizeof(Md_Ast))) == NULL)
        return NULL;

    // TODO: this is a place holder. For a fucntion that processes markdown text, it inherently
    // needs to be a parent of some kind, the onus is on the caller to recongize this
    ast_node->node_type = MD_ROOT_NODE;

    size_t src_len = strlen(source);

    // printf("\"%s\" %zu\n", source, src_len);

    char text_buffer[MD_MAX_TEXT_BUFFER] = {0};
    size_t text_buffer_pos = 0;

    for (size_t i = *pos; i <= src_len; ++i) {
        if (source[i] == '~' && i + 1 < src_len && source[i + 1] == '~') {
            size_t sub_pos = i + 2;
            Md_Ast *del_node = kevlar_md_process_pair(source, src_len, "~~", &sub_pos, MD_DEL_NODE,
                                                      allow_line_breaks, true, true);
            if (del_node) {
                if (text_buffer_pos > 0) {
                    Md_Ast *txt_node = create_text_node_from_buffer(text_buffer, text_buffer_pos);
                    kevlar_md_ast_child_append(ast_node, txt_node);

                    memset(text_buffer, 0, text_buffer_pos);
                    text_buffer_pos = 0;
                }

                *pos = sub_pos;
                i = *pos;

                kevlar_md_ast_child_append(ast_node, del_node);
                continue;
            }
        } else if (source[i] == '[') {
            size_t sub_pos = i + 1;
            Md_Ast *link_title_component =
                kevlar_md_process_pair(source, src_len, "]", &sub_pos, MD_LINK_NODE, false, true, true);

            if (source[sub_pos + 1] != '(')
                free(link_title_component);

            if (link_title_component != NULL && source[sub_pos + 1] == '(') {
                size_t sub_pos_pre = sub_pos + 2;

                Md_Ast *href_text_node = kevlar_md_process_pair(
                    source, src_len, ")", &sub_pos_pre, MD_ROOT_NODE, allow_line_breaks, false, true);

                if (href_text_node) {
                    if (href_text_node->c_count > 0) {
                        link_title_component->opt.link_opt.href_len = sub_pos_pre - sub_pos - 2;
                        link_title_component->opt.link_opt.href_str =
                            strndup(href_text_node->children[0]->opt.text_opt.data,
                                    href_text_node->children[0]->opt.text_opt.len);
                    }
                    kevlar_md_free_ast(href_text_node);

                    _md_handle_escaping(&link_title_component->opt.link_opt.href_str,
                                        &link_title_component->opt.link_opt.href_len);

                    if (text_buffer_pos > 0) {
                        Md_Ast *txt_node =
                            create_text_node_from_buffer(text_buffer, text_buffer_pos);
                        kevlar_md_ast_child_append(ast_node, txt_node);

                        memset(text_buffer, 0, text_buffer_pos);
                        text_buffer_pos = 0;
                    }

                    kevlar_md_ast_child_append(ast_node, link_title_component);

                    *pos = sub_pos_pre;
                    i = *pos;

                    continue;
                }
            }
        } else if (source[i] == '`') {
            size_t repeating_count = utl_count_repeating_char('`', &source[i]);
            if (repeating_count <= 3) {
                char *suffix = malloc(sizeof(char) * repeating_count + 1);
                memset(suffix, '`', repeating_count);
                suffix[repeating_count] = '\0';

                size_t sub_pos = i + repeating_count;
                Md_Ast *inline_code_block =
                    kevlar_md_process_pair(source, src_len, suffix, &sub_pos, MD_INLINE_CODE_BLOCK,
                                           allow_line_breaks, false, true);

                if (inline_code_block) {
                    if (text_buffer_pos > 0) {
                        Md_Ast *txt_node =
                            create_text_node_from_buffer(text_buffer, text_buffer_pos);
                        kevlar_md_ast_child_append(ast_node, txt_node);

                        memset(text_buffer, 0, text_buffer_pos);
                        text_buffer_pos = 0;
                    }

                    *pos = sub_pos;
                    i = *pos;

                    kevlar_md_ast_child_append(ast_node, inline_code_block);
                    free(suffix);
                    continue;
                }

                free(suffix);
            }
        } else if (source[i] == '_') {
            size_t sub_pos = i + 1;
            Md_Ast *em_node = kevlar_md_process_pair(source, src_len, "_", &sub_pos, MD_EM_NODE,
                                                     allow_line_breaks, true, true);
            if (em_node) {
                if (text_buffer_pos > 0) {
                    Md_Ast *txt_node = create_text_node_from_buffer(text_buffer, text_buffer_pos);
                    kevlar_md_ast_child_append(ast_node, txt_node);

                    memset(text_buffer, 0, text_buffer_pos);
                    text_buffer_pos = 0;
                }

                *pos = sub_pos;
                i = *pos;

                kevlar_md_ast_child_append(ast_node, em_node);
                continue;
            }
        } else if (source[i] == '*') {
            size_t sub_pos = i + 1;
            char *suffix = "*";
            NodeType node_type = MD_EM_NODE;

            if (i + 2 < src_len && source[i + 1] == '*' && source[i + 2] == '*') {
                suffix = "***";
                sub_pos = i + 3;
                node_type = MD_STRONG_EM_NODE;
            } else if (i + 1 < src_len && source[i + 1] == '*') {
                suffix = "**";
                sub_pos = i + 2;
                node_type = MD_STRONG_NODE;
            }

            Md_Ast *em_or_strong_node = kevlar_md_process_pair(source, src_len, suffix, &sub_pos,
                                                               node_type, allow_line_breaks, true, true);
            if (em_or_strong_node) {

                if (text_buffer_pos > 0) {
                    Md_Ast *txt_node = create_text_node_from_buffer(text_buffer, text_buffer_pos);
                    kevlar_md_ast_child_append(ast_node, txt_node);

                    memset(text_buffer, 0, text_buffer_pos);
                    text_buffer_pos = 0;
                }

                *pos = sub_pos;
                i = *pos;

                kevlar_md_ast_child_append(ast_node, em_or_strong_node);
                continue;
            }
        } else if (source[i] == '\n') {
            Md_Line_End_Type line_end = get_line_end_type(source, i);

            // All the situations where we can no longer proceed
            if ((line_end == MD_SINGLE_LINE_BREAK && !allow_line_breaks) ||
                line_end == MD_DOUBLE_LINE_BREAK || line_end == MD_EOF) {

                *pos = line_end == MD_DOUBLE_LINE_BREAK ? i + 1 : i;
                i = *pos;

                Md_Ast *txt_node = create_text_node_from_buffer(text_buffer, text_buffer_pos);
                kevlar_md_ast_child_append(ast_node, txt_node);

                // TODO: remove this down the line
                assert(strlen(txt_node->opt.text_opt.data) == text_buffer_pos &&
                       "If this fails, it means some memory corruption has happened within the "
                       "text");

                goto return_ast_and_exit;
            }
        }

        if (source[i] == '\\' && i + 1 < src_len &&
            (SPECIAL_CHAR_SET[(unsigned char)source[i + 1]])) {
            text_buffer[text_buffer_pos] = source[i + 1];
            text_buffer_pos++;

            i++;
            (*pos) += 2;
            continue;
        }

        if (source[i] == '\0')
            break;

        text_buffer[text_buffer_pos] = source[i];
        text_buffer_pos++;
        (*pos)++;
    }

    size_t strip_offset = utl_lstrip_offset(text_buffer, text_buffer_pos);
    if (strip_offset == text_buffer_pos)
        return ast_node;

    // printf("\"%s\" %zu\n", text_buffer, text_buffer_pos);

    Md_Ast *txt_node = create_text_node_from_buffer(text_buffer, text_buffer_pos);
    kevlar_md_ast_child_append(ast_node, txt_node);

    return ast_node;

return_ast_and_exit:
    return ast_node;
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

            Md_Ast *root_txt_node =
                kevlar_md_process_text_node(source, pos, /* allow_line_breaks */ false);

            if (root_txt_node == NULL)
                return ast_node;

            ast_node->children = root_txt_node->children;
            ast_node->c_count = root_txt_node->c_count;

            free(root_txt_node);

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
        } else if (source[i] == '`' && (i + 1 < src_len && source[i + 1] == '`') &&
                   (i + 2 < src_len && source[i + 2] == '`')) {

            size_t pos = i + 2;
            size_t newline_offset;

            char *newline_char = memchr(&source[pos], '\n', src_len);
            if (newline_char != NULL) {
                newline_offset = (newline_char - &source[pos]) - 1;

                char *code_lang_str = NULL;
                if (newline_offset > 0) {
                    code_lang_str = malloc(sizeof(char) * newline_offset);
                    strncpy(code_lang_str, &source[pos+1], newline_offset);
                }

                pos += newline_offset + 2;

                Md_Ast* code_block = kevlar_md_process_pair(source, src_len, "\n```", &pos, MD_CODE_BLOCK, false, false, false);
                if (code_block) {
                    kevlar_md_ast_child_append(ast, code_block);
                    code_block->opt.code_opt.lang_str = code_lang_str;
                    code_block->opt.code_opt.lang_str_len = newline_offset;

                    i = pos + 2;
                    continue;
                }

                free(code_lang_str);
            }
        }

        Md_Ast *root_text_node =
            kevlar_md_process_text_node(source, &i, /* allow_line_breaks */ true);

        if (!root_text_node)
            kevlar_err("Could not process text node at position %zu\n", i);

        root_text_node->node_type = MD_PARA_NODE;
        kevlar_md_ast_child_append(ast, root_text_node);
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
