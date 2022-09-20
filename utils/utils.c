#include <stdio.h>
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

void utl_strstr(char * haystack, char * needle) {
  printf("%s\n", strchr(haystack, needle[0]));
  //for (int i = 0; needle[i] != '\0'; i++) { }
}

void utl_prepend_str(char prefix[], char str[]) {
  char temp_str[strlen(str)+1];
  strcpy(temp_str, str);
  strcpy(str, prefix);
  strcat(str, temp_str);
}

// https://stackoverflow.com/questions/2328182/prepending-to-a-string
void utl_prepend(char* string, const char* prefix)
{
    size_t len = strlen(prefix);
    memmove(string + len, string, strlen(string) + 1);
    memcpy(string, prefix, len);
}

void utl_truncateLast(char *str) {
  str[strlen(str)-1] = '\0';
}
