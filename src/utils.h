#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

void utl_prepend_str(char prefix[], char str[]);

void utl_prepend(char *string, const char *prefix);

size_t utl_lstrip_offset(char* data, size_t length);

void utl_truncateLast(char *str);
char *utl_get_date_time_unsafe();

char *utl_camel_case_to_spaces(char *input, char *output);

char *utl_spaces_to_dash_case(char *input, char *output);

size_t utl_count_repeating_char(char chr, const char *str);

void utl_visualize_ast(Md_Ast* ast, int level);

void utl_mkdir_crossplatform(char *folder_path);

#endif
