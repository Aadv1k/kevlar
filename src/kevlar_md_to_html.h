#ifndef KEVLAR_MD_TO_HTML_H
#define KEVLAR_MD_TO_HTML_H

#define MD_HEADING_LEVEL 6
#define MD_HR_LENGTH 3

#define MD_LINK_SIZE 1024

// TODO: Artifact of previous implementation
#define RST_LINE_LENGTH 10000

void md_parse(char *in_file_path, char *out_file_path);

#endif
