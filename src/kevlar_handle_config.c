#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "kevlar_errors.h"
#include "kevlar_handle_config.h"
#include "kevlar_new.h"

void kevlar_get_opt_from_config(char *file_path, char *opt, char *arg) {
  FILE *file_buf;
  if ((file_buf = fopen(file_path, "r")) == NULL) {
    fprintf(stderr, "[kevlar] something went wrong while opening %s\n", file_path);
    exit(1);
  }

  int line_count = 0;
  char cur_line[CONFIG_MAX_FILE_SIZE];

  char curOpt[CONFIG_MAX_OPT_SIZE];
  strcpy(curOpt, opt);

  for (int i = 0; opt[i] != '\0'; i++)
    curOpt[i] = tolower(opt[i]);

  while (fgets(cur_line, CONFIG_MAX_FILE_SIZE, file_buf) != NULL) {
    char command[2][CONFIG_MAX_OPT_SIZE] = {};

    // NOTE: Only line comments for now
    if (cur_line[0] == '#')
      continue;

    utl_truncateLast(cur_line);
    strcpy(command[1], strrchr(cur_line, '=') + 1);
    *strchr(cur_line, '=') = '\0';
    strcpy(command[0], cur_line);

    for (int i = 0; command[0][i] != '\0'; i++)
      command[0][i] = tolower(command[0][i]);

    if (strcmp(command[0], curOpt) == 0) {
      strcpy(arg, command[1]);
      break;
    }
  }
  fclose(file_buf);
}

void kevlar_generate_skeleton_config(char file_path[CONFIG_MAX_PATH_SIZE]) {
  FILE *file_buf = fopen(file_path, "w");
  if (!file_buf)
    kevlar_err("[kevlar] something went wrong while generating sample config\n");
  // This works for now since the file we will be generating won't be that big
  fprintf(file_buf, "author=John\ntitle=My example site\ntheme=kyudo\n");
  fclose(file_buf);
}
