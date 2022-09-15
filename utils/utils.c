#include <stdlib.h>
#include <string.h>

char * utl_strchrev(char str[], char c) {
  for (int i = 1; &str[strlen(str)-i] != &str[0]; i++)  {
    if (str[strlen(str)-i] == c) {
      return &str[strlen(str)-i]; 
    }
  }
  return str;
}

void utl_prepend_str(char prefix[], char str[]) {
  char *temp_str = strdup(str);
  strcpy(str, prefix);
  strcat(str, temp_str);
  free(temp_str);
}

void utl_truncateLast(char *str) {
  str[strlen(str)-1] = '\0';
}
