#include "../src/kevlar_rst_to_html.h"
#include "../utils/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RST_MAX_PATH_SIZE 128

void usage_exit() {
  fprintf(stderr, "USAGE: rst2html INPUT.rst OUTPUT.html\n");
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    usage_exit();
  }

  char in_file_path[RST_MAX_PATH_SIZE], out_file_path[RST_MAX_PATH_SIZE];

  strcpy(in_file_path, argv[1]);
  strcpy(out_file_path, argv[2]);

  if (strcmp(strrchr(in_file_path, '.'), ".rst") != 0) {
    fprintf(stderr, "[rst2html] %s doesn't seem to be a rst file\n", in_file_path);
    exit(1);
  }

  rst_parse(in_file_path, out_file_path);

  return 0;
}
