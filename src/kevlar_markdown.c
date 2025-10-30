#include "kevlar_markdown.h"
#include "kevlar_errors.h"
#include "utils.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MD_MAX_TEXT_BUFFER 5120 // 5 MiB, should be enough for most?

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

Md_Ast* create_text_node_from_buffer(char* buffer, size_t buffer_len) {
    Md_Ast* txt_node = malloc(sizeof(Md_Ast));

    txt_node->node_type = MD_TEXT_NODE;
    txt_node->opt.text_opt.len = buffer_len;

    txt_node->opt.text_opt.data = malloc(sizeof(char) * buffer_len);
    strncpy(txt_node->opt.text_opt.data, buffer, buffer_len);

    return txt_node;
}

Md_Ast *kevlar_md_process_text_node(const char *source, size_t *pos, bool allow_line_breaks) {
    Md_Ast *ast_node;
    if ((ast_node = malloc(sizeof(Md_Ast))) == NULL)
        return NULL;

    // TODO: this is a place holder. For a fucntion that processes markdown text, it inherently needs to be a parent of some kind, the onus is on the caller to recongize this
    ast_node->node_type = MD_ROOT_NODE;

    size_t src_len = strlen(source);

    char text_buffer[MD_MAX_TEXT_BUFFER] = {0};
    size_t text_buffer_pos = 0;

    for (size_t i = *pos; i <= src_len; ++i) {
        switch (source[i]) {

        case '\n': {
            Md_Line_End_Type line_end = get_line_end_type(source, i);

            // All the situations where we can no longer proceed
            if ((line_end == MD_SINGLE_LINE_BREAK && !allow_line_breaks) ||
                line_end == MD_DOUBLE_LINE_BREAK || line_end == MD_EOF
            ) {

                *pos = line_end == MD_DOUBLE_LINE_BREAK ? i + 1 : i;

                Md_Ast* txt_node = create_text_node_from_buffer(text_buffer, text_buffer_pos);
                kevlar_md_ast_child_append(ast_node, txt_node);


                goto return_ast_and_exit;
            }
        }

        default: {
            text_buffer[text_buffer_pos] = source[i];
            if (source[i] != '\0') {
                text_buffer_pos++;
            }
        }
        }
    }


    size_t strip_offset = utl_lstrip_offset(text_buffer, text_buffer_pos);
    if (strip_offset == text_buffer_pos) return ast_node;

    Md_Ast* txt_node = create_text_node_from_buffer(text_buffer, text_buffer_pos);
    kevlar_md_ast_child_append(ast_node, txt_node);

    return ast_node;

return_ast_and_exit:
    return ast_node;
}

#if 0

Md_Ast *kevlar_md_process_text_node(const char *source, size_t *pos, bool allow_line_breaks) {

    Md_Ast *ast_node;
    if ((ast_node = malloc(sizeof(Md_Ast))) == NULL)
        return NULL;

    ast_node->node_type = allow_line_breaks ? MD_PARA_NODE : MD_TEXT_NODE;

    size_t src_len = strlen(source);
    char text_buffer[MD_MAX_TEXT_BUFFER] = {0};

    size_t text_buffer_pos = 0;

    for (size_t i = *pos; i <= src_len; ++i) {
        switch (source[i]) {
        case '*': {
            if (i + 1 < src_len && source[i + 1] != '*') {
                size_t sub_cur = 0;

                size_t cur = 0;
                for (cur = i + 1; cur < src_len; ++cur) {
                    if (source[cur] == '*')
                        break;
                }

                if (cur == 0)
                    assert(false && "Unreachable as this would be caught in the above loop");
                // if (cur == src_len - 1) break;

                // TODO: clean this logic up, and make it not hard coded like this
                /******************************* */
                assert(allow_line_breaks);
                Md_Ast *sibling_txt_node = malloc(sizeof(Md_Ast));
                sibling_txt_node->node_type = MD_TEXT_NODE;

                if ((sibling_txt_node->opt.text_opt.data =
                         malloc(sizeof(char) * text_buffer_pos)) == NULL) {
                    free(ast_node);
                    kevlar_err("malloc of size %zu failed", sizeof(char) * text_buffer_pos);
                }

                strncpy(sibling_txt_node->opt.text_opt.data, text_buffer, text_buffer_pos);
                sibling_txt_node->opt.text_opt.len = text_buffer_pos;

                kevlar_md_ast_child_append(ast_node, sibling_txt_node);
                /******************************* */

                char *tmp_buf;
                size_t tmp_buf_size = cur - (i + 1);
                tmp_buf = malloc(sizeof(char) * tmp_buf_size);

                strncpy(tmp_buf, &source[i + 1], tmp_buf_size);

                sub_cur = 0;
                Md_Ast *em_node = kevlar_md_process_text_node(tmp_buf, &sub_cur, true);
                if (em_node == NULL) {
                    free(ast_node);
                    free(tmp_buf);

                    kevlar_err("Failed to parse content inside asterisks");
                }
                em_node->node_type = MD_EM_NODE;
                kevlar_md_ast_child_append(ast_node, em_node);

                *pos = cur + 1;
                i = cur;
                free(tmp_buf);

                break;
            }
        }
        case '_':
        case '`':
        case '[':
        case '<':
        default: {
            printf("\"%c\"\n", source[i]);
            // if at last position
            // # The quick brown fox \n
            if (i == src_len || source[i] == '\n') {
                Md_Line_End_Type line_end = get_line_end_type(source, i);
                text_buffer[text_buffer_pos] = '\0';

                switch (line_end) {
                case MD_SINGLE_LINE_BREAK:
                    if (allow_line_breaks)
                        break;
                    *pos = i + 1;
                case MD_DOUBLE_LINE_BREAK:
                    *pos = i + 2;
                case MD_EOF:
                    *pos = i;
                }

                // // TODO: should we return an empty paragraph node only?
                // if (utl_lstrip_offset(text_buffer, text_buffer_pos) == text_buffer_pos) {
                // 	free(ast_node);
                // 	return NULL;
                // }

                // Md_Ast* txt_node = ast_node;
                // if (allow_line_breaks) {
                //     txt_node = malloc(sizeof(Md_Ast));
                // 	txt_node->node_type = MD_TEXT_NODE;
                // }

                // if ((txt_node->opt.text_opt.data = malloc(sizeof(char) * text_buffer_pos)) ==
                // NULL) { 	free(ast_node); 	kevlar_err("malloc of size %zu failed",
                // sizeof(char) * text_buffer_pos);
                // }

                // strncpy(txt_node->opt.text_opt.data, text_buffer, text_buffer_pos);
                // txt_node->opt.text_opt.len = text_buffer_pos;

                // if (allow_line_breaks) kevlar_md_ast_child_append(ast_node, txt_node);

                // return ast_node;
            }

            text_buffer[text_buffer_pos] = source[i];
            text_buffer_pos++;
        }
        }
    }

    /*
            The quick _hello_ fox jumps over *World*
            Hello world
    */

    if (ast_node->c_count == 0 && text_buffer_pos != 0) {
        if (utl_lstrip_offset(text_buffer, text_buffer_pos) == text_buffer_pos) {
            free(ast_node);
            return NULL;
        }

        Md_Ast *txt_node = ast_node;
        if (allow_line_breaks) {
            txt_node = malloc(sizeof(Md_Ast));
            txt_node->node_type = MD_TEXT_NODE;
        }

        if ((txt_node->opt.text_opt.data = malloc(sizeof(char) * text_buffer_pos)) == NULL) {
            free(ast_node);
            kevlar_err("malloc of size %zu failed", sizeof(char) * text_buffer_pos);
        }

        strncpy(txt_node->opt.text_opt.data, text_buffer, text_buffer_pos);
        txt_node->opt.text_opt.len = text_buffer_pos;

        if (allow_line_breaks)
            kevlar_md_ast_child_append(ast_node, txt_node);
    }

    return ast_node;
}
#endif

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
                Md_Ast* txt_node = ast_node->children[0];

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
    (void)source;
    Md_Ast *ast;
    if ((ast = malloc(sizeof(Md_Ast))) == NULL) {
        return NULL;
    }

    ast->node_type = MD_ROOT_NODE;
    ast->children = NULL;
    ast->c_count = 0;

    size_t src_len = strlen(source);

    for (size_t i = 0; i < src_len; ++i) {
        switch (source[i]) {
        case '#': {
            Md_Ast *h_node;

            if ((h_node = kevlar_md_process_heading_node(source, &i)) != NULL) {
                kevlar_md_ast_child_append(ast, h_node);
                break;
            }
        }

        default: {
            Md_Ast *root_text_node =
                kevlar_md_process_text_node(source, &i, /* allow_line_breaks */ true);

            if (!root_text_node)
                kevlar_err("Could not process text node at position %zu\n", i);

            root_text_node->node_type = MD_PARA_NODE;
            kevlar_md_ast_child_append(ast, root_text_node);
        }
        }
    }

    return ast;
}

void kevlar_md_free_ast(Md_Ast *ast) {
    for (size_t i = 0; i < ast->c_count; ++i) {
        if (ast->children[i]->node_type == MD_TEXT_NODE) {
            free(ast->children[i]->opt.text_opt.data);
            free(ast->children[i]);
            continue;
        }

        kevlar_md_free_ast(ast->children[i]);
    }

    free(ast);
}
