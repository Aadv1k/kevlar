#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/kevlar_rst_to_html.h"
#include "../utils/utils.h" 

void usage_sub() {
  puts("kev_rst2html -[OPT] INPUT.rst OUTPUT.html\n\t-h -- On invalid rst exit "
       "with error message\n");
  exit(1);
}

int main(int argc, char *argv[]) {
  int ERRORS;

  if (argc < 3) {
    usage_sub();
  }

  char in_file_path[128], out_file_path[128];

  int opt;
  while ((opt = getopt(argc, argv, ":h:")) != -1) {
    switch (opt) {
    case 'h':
      ERRORS = true;
      strcpy(in_file_path, optarg);
      strcpy(out_file_path, argv[optind]);
      break;
    case ':':
    case '?':
      usage_sub();
      break;
    }
  }

  if (!ERRORS) {
    strcpy(in_file_path, argv[1]);
    strcpy(out_file_path, argv[2]);
  }

  if (strcmp(utl_strchrev(in_file_path, '.'), ".rst") != 0) {
    fprintf(stderr, "[rst2html] %s doesn't seem to be a rst file\n", in_file_path);
    exit(1);
  }

  rst_parse(in_file_path, out_file_path);

  return 0;
}

