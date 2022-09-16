#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kevlar_handle_templates.h"
#include "../utils/utils.h"

void kevlar_write_html_string(char line[], char substr[], char content[], FILE * out_file_buffer) {
  char tail[TEMPLATE_MAX_TAG_SIZE];
  strcpy(tail, utl_strchrev(strstr(line, substr), '-')+1);
  *(strchr(line, '-')) = '\0';

  line[strlen(line)-1] == ' ' 
    ? line[strlen(line)-1] = '\0' 
    : line[strlen(line-1)];
  
  fprintf(out_file_buffer, "%s %s %s", line, content, tail);
};

void kevlar_parse_template(char file_path[CONFIG_MAX_PATH_SIZE], char out_file_path[CONFIG_MAX_PATH_SIZE], KevlarConfig *kev_config) {
  FILE * file_buffer;
  FILE * out_file_buffer;

  file_buffer = fopen(file_path, "r");
  out_file_buffer = fopen(out_file_path, "w");

  if (out_file_buffer == NULL || file_buffer == NULL) {

    file_buffer == NULL 
      ? fprintf(stderr, "[kevlar] ran into a problem while opening %s; it may not exist\n", file_path) 
      : fprintf(stderr, "[kevlar] ran into a problem while opening %s\n", out_file_path);
    exit(1);
   };

  char *ext = utl_strchrev(file_path, '.');

  if (strcmp(ext, ".html") != 0 && strcmp(ext, ".htm") != 0) {
    fprintf(stderr, "[kevlar] attempt to open file with invalid extension %s; can only open html files\n", file_path);
    exit(1);
  }

  char file[CONFIG_MAX_FILE_SIZE];
  char line[TEMPLATE_MAX_LINE_SIZE];

  while ((fgets(line, TEMPLATE_MAX_LINE_SIZE, file_buffer)) != NULL) {
    if (strstr(line, "--TITLE--")) { kevlar_write_html_string(line, "--TITLE--", kev_config->configTitle, out_file_buffer);
      continue;
    } else if (strstr(line, "--AUTHOR--")) {
      kevlar_write_html_string(line, "--AUTHOR--", kev_config->configAuthor, out_file_buffer);
      continue;
    } 
    fprintf(out_file_buffer, "%s", line);
  }

  fclose(file_buffer);
};
