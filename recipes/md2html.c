#include "../src/kevlar_md_to_html.h"
#include "../utils/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MD_MAX_PATH_SIZE 128

void usage_exit() {
  fprintf(stderr, "USAGE: md2html INPUT.md OUTPUT.html\n");
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    usage_exit();
  }

  char in_file_path[MD_MAX_PATH_SIZE], out_file_path[MD_MAX_PATH_SIZE];

  strcpy(in_file_path, argv[1]);
  strcpy(out_file_path, argv[2]);

  if (strcmp(utl_strchrev(in_file_path, '.'), ".md") != 0) {
    fprintf(stderr, "[md2html] %s doesn't seem to be a markdown file\n", in_file_path);
    exit(1);
  }

  md_parse(in_file_path, out_file_path);

  return 0;
}
