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

// https://stackoverflow.com/questions/2328182/prepending-to-a-string
void utl_prepend(char *string, const char *prefix) {
  size_t len = strlen(prefix);
  memmove(string + len, string, strlen(string) + 1);
  memcpy(string, prefix, len);
}

void utl_truncateLast(char *str) { str[strlen(str) - 1] = '\0'; }

void utl_mkdir_crossplatform(char *folder_path) {
#if defined(_WIN32)
  CreateDirectory(folder_path, NULL);
#else
  // It is quite unlikely that this might run into an error; We want to ignore
  // the EEXIST error anyways.
  mkdir(folder_path, 0777);
#endif
}
