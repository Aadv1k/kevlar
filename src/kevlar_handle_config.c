#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kevlar_handle_config.h"

void kevlar_load_config(char file_path[CONFIG_MAX_PATH_SIZE], KevlarConfig* config_struct) {
  FILE * file_buf;

  if (!(file_buf = fopen(file_path, "r"))) {
    fprintf(stderr, "[kevlar] something went wrong while opening %s\n", file_path);
    exit(1);
  }

  int line_count = 0;
  while (!feof(file_buf)) {
    char cur_line[CONFIG_MAX_FILE_SIZE];
    char *target;
    fgets(cur_line, CONFIG_MAX_FILE_SIZE, file_buf);
    target = strtok(cur_line, "=");
    char command[2][CONFIG_MAX_OPT_SIZE] = {};

    // Only line comments for now
    if (cur_line[0] == '#') continue;

    int i = 0;
    while (target != NULL) {
      if (i > 1 || strlen(target) == 1) {
        fprintf(stderr, "[kevlar] invalid config at %s at line %d\n", file_path, line_count+1); 
        exit(1); 
      } 
      strcpy(command[i], target);
      target = strtok(NULL, "=");
      i++;
    }

    if (strcmp(command[0], "author") == 0) {
      strcpy(config_struct->configAuthor, command[1]);
    } else if (strcmp(command[0], "title") == 0) { 
      strcpy(config_struct->configTitle, command[1]);
    } 

    if (strcmp(command[0], "rst_loader") == 0) {
      strcpy(config_struct->configRstLoader, command[1]);
    } else if (strcmp(command[0], "markdown_loader") == 0) {
      strcpy(config_struct->configMarkdownLoader, command[1]);
    }

    if (strlen(config_struct->configMarkdownLoader) == 0) {
      strcpy(config_struct->configMarkdownLoader, "./md2html");
    } else if (strlen(config_struct->configRstLoader) == 0) {
      strcpy(config_struct->configMarkdownLoader, "./rst2html");
    }

    line_count++;
  }
};

void kevlar_generate_skeleton_config(char file_path[CONFIG_MAX_PATH_SIZE]) {
  FILE * file_buf;  
  if ((file_buf = fopen(file_path, "w")) == NULL) {
    fprintf(stderr, "[kevlar] something went wrong while generating sample config\n");
    exit(1);
  };
  
  // This works for now since the file we will be generating won't be that big
  fprintf(file_buf, "author=John doe\ntitle=My example site\n# markdown_loader=\n# rst_loader=");
}
