#ifndef KEVLAR_RST_TO_HTML_H
#define KEVLAR_RST_TO_HTML_H

#define RST_LINE_LENGTH 10000
#define RST_FIELD_LIST_SIZE 50

void rst_parse(char *rst_file_path, char *html_file_path);

long rst_get_file_length(char *filename);

#endif
