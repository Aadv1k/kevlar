#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#if __has_include("windows.h")
#include <windows.h>
#endif

void utl_prepend_str(char prefix[], char str[]) {
  char temp_str[strlen(str) + 1];
  strcpy(temp_str, str);
  strcpy(str, prefix);
  strcat(str, temp_str);
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

char *utl_spaces_to_dash_case(char *input, char *output) {
  x_case_to_y_case(input, output, " ", "-");
  return output;
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
