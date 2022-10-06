#ifndef UTILS_H
#define UTILS_H

char * utl_strchrev(char str[], char c);

void utl_prepend_str(char prefix[], char str[]);

void utl_prepend(char* string, const char* prefix);

void utl_truncateLast(char *str);

void utl_mkdir_crossplatform(char * folder_path);

#endif
