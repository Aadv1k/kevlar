#ifndef KEVLAR_RST_TO_HTML_H
#define KEVLAR_RST_TO_HTML_H

#include <stdio.h>

#define RST_LINE_LENGTH 10000
#define RST_FIELD_LIST_SIZE 50

void rst_parse(char *rst_file_path, char *html_file_path);

#endif
