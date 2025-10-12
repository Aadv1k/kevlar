#include "kevlar_markdown.h"
#include "kevlar_errors.h"
#include "utils.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Md_Line_End_Type get_line_end_type(const char *source, size_t pos) {
	size_t len = strlen(source);

	if (pos + 1 > len) return MD_EOF;

	if (source[pos] == '\n' && source[pos + 1] == '\n')
		return MD_DOUBLE_LINE_BREAK;

	return MD_SINGLE_LINE_BREAK;
}


void kevlar_md_ast_child_append(Md_Ast *parent, Md_Ast *child) {
    Md_Ast** a;
	if ((a = realloc(parent->children, sizeof(Md_Ast *) * parent->c_count + 1)) == NULL) {
        kevlar_err("Could not re-allocate memory!");
    }

    parent->children = a;
	parent->children[parent->c_count] = child;
	parent->c_count++;
}

Md_Ast *kevlar_md_process_text_node(const char *source, size_t *pos, bool allow_line_break) {
	Md_Ast *ast_node;
	if ((ast_node = malloc(sizeof(Md_Ast))) == NULL) {
		return NULL;
	}

	ast_node->node_type = MD_TEXT_NODE;

	size_t src_len = strlen(source);

	for (size_t i = *pos; i <= src_len; ++i) {
		if (i + 1 > src_len || source[i] == '\n') {
			size_t offset = 0;
			size_t cursor_jmp;

			Md_Line_End_Type line_end = get_line_end_type(source, i);

			switch (line_end) {
			case MD_SINGLE_LINE_BREAK: {
				if (allow_line_break) break;
				offset = i - *pos;
				cursor_jmp = i;
                break;
			}
			case MD_DOUBLE_LINE_BREAK: {
				// # foo \n bar bbaz \n\n
				offset = i - 1 - *pos;
				cursor_jmp = i + 1;
                break;
			}
			case MD_EOF: {
				// # example
				offset = i - *pos;
				cursor_jmp = i;
                break;
			}
			default: /* Unreachable */
                 goto parsing_error;
			}

            if (offset == 0) 
                goto parsing_error;

			if ((ast_node->opt.text_opt.data = malloc(sizeof(char) * offset)) == NULL) 
                goto parsing_error;

			ast_node->opt.text_opt.len = offset;

			strncpy(ast_node->opt.text_opt.data, (source + (*pos)), offset);

			*pos = cursor_jmp;
            return ast_node;
		}
	}


parsing_error:
	free(ast_node);
	return NULL;
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
			if (level > 6) goto parsing_error;
			ast_node->opt.h_opt.level = level;

			(*pos) = i + 1; // accounting for the space as well

			if (i >= src_len)
				return ast_node;

			Md_Ast *txt_node = kevlar_md_process_text_node(source, pos, /* allow_line_break */ false);
			if (txt_node == NULL)
				return ast_node;

             size_t left_offset = utl_lstrip_offset(txt_node->opt.text_opt.data, txt_node->opt.text_opt.len);
             size_t new_len = txt_node->opt.text_opt.len - left_offset;

             if (left_offset) {
                char* new_data = malloc(sizeof(char) * new_len);
                strncpy(new_data, txt_node->opt.text_opt.data + left_offset, new_len);
                free(txt_node->opt.text_opt.data);

                txt_node->opt.text_opt.data = new_data;
                txt_node->opt.text_opt.len = new_len;
             }

			kevlar_md_ast_child_append(ast_node, txt_node);

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
			Md_Ast *txt_node = kevlar_md_process_text_node(source, &i, /* allow_line_break */ true);
			if (!txt_node)
				kevlar_err("Could not process text node at position %zu\n", i);


			// TODO: here wrap this with paragraph
			kevlar_md_ast_child_append(ast, txt_node);
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
