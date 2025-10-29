#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>

#include "kevlar_markdown.h"

#if __has_include("windows.h")
#include <windows.h>
#endif

void utl_prepend_str(char prefix[], char str[]) {
  char temp_str[strlen(str) + 1];
  strcpy(temp_str, str);
  strcpy(str, prefix);
  strcat(str, temp_str);
}

size_t utl_lstrip_offset(char* data, size_t length) {
    // "        foo bar" -> "foo bar"
    
    size_t end_p = 0;

    for (size_t i = 0; i < length; ++i) {
        if (isspace(data[i]) != 0) end_p++;
        if (isspace(data[i]) == 0) break;
    }

    return end_p;
}

size_t utl_count_repeating_char(char chr, const char *str) {
  for (size_t i = 0; str[i] != '\0'; i++) {
    if (str[i] != chr)
      return i;
  }
  return strlen(str);
}

// https://stackoverflow.com/questions/2328182/prepending-to-a-string
void utl_prepend(char *string, const char *prefix) {
  size_t len = strlen(prefix);
  memmove(string + len, string, strlen(string) + 1);
  memcpy(string, prefix, len);
}

char * utl_get_date_time_unsafe() {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  char * time_str = malloc(100 * sizeof(char));
  sprintf(time_str, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  return time_str;
}

void utl_truncateLast(char *str) { str[strlen(str) - 1] = '\0'; }

void x_case_to_y_case(char *input, char *output, const char *x, const char *y) {
  output[0] = '\0';


  char *word;
  word = strtok(input, x);

  while (word != NULL) {
    strcat(output, word);
    strcat(output, y);
    word = strtok(NULL, x);
  }

  utl_truncateLast(output);
}

// TODO: its not "camel case" its dashcase
char *utl_camel_case_to_spaces(char *input, char *output) {
  x_case_to_y_case(input, output, "-", " ");
  return output;
}

void utl_spaces_to_dash_case(char *input, char *output) {
  x_case_to_y_case(input, output, " ", "-");
}

void utl_mkdir_crossplatform(char *folder_path) {
#if defined(_WIN32)
  CreateDirectory(folder_path, NULL);
#else
  // It is quite unlikely that this might run into an error; We want to ignore
  // the EEXIST error anyways.
  mkdir(folder_path, 0777);
#endif
}


const char* _node_type_to_str(NodeType type) {
    switch (type) {
        case MD_ROOT_NODE:    return "MD_ROOT_NODE";
        case MD_PARA_NODE:    return "MD_PARA_NODE";
        case MD_EM_NODE:      return "MD_EM_NODE";
        case MD_STRONG_NODE:  return "MD_STRONG_NODE";
        case MD_DEL_NODE:     return "MD_DEL_NODE";
        case MD_TEXT_NODE:    return "MD_TEXT_NODE";
        case MD_HEADING_NODE: return "MD_HEADING_NODE";
        default:              return "UNKNOWN_NODE_TYPE";
    }
}

void utl_visualize_ast(Md_Ast* ast, int spaces) {

    char* indent = malloc(sizeof(char) * spaces);
    memset(indent, ' ', spaces*2);

    printf("%s%s\n", indent, _node_type_to_str(ast->node_type));
    if (ast->node_type == MD_TEXT_NODE) {
        printf("%s└ Data: %s\n", indent, ast->opt.text_opt.data);
    }

    if (ast->node_type == MD_HEADING_NODE) {
        printf("%s└ Level: %d\n", indent, ast->opt.h_opt.level);
    }

    for (size_t i = 0; i < ast->c_count; ++i) {
        utl_visualize_ast(ast->children[i], spaces + 1);
    }

    free(indent);
}