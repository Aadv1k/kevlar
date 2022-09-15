#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kevlar_handle_config.h"

#define TEMPLATE_MAX_TAG_SIZE 64
#define TEMPLATE_MAX_LINE_SIZE 224

// TODO: Put this in a util file This can be implemented in rst2html, ans kevlar_handle_config.c as well
char * strchrev(char string[], char target) {
  for (int i = 1; &string[strlen(string)-i] != &string[0]; i++)  {
    if (string[strlen(string)-i] == target) {
      return &string[strlen(string)-i]; 
    }
 }
  return string;
}

void kevlar_write_html_string(char line[], char substr[], char content[], FILE * out_file_buffer) {
  char tail[TEMPLATE_MAX_TAG_SIZE];
  strcpy(tail, strchrev(strstr(line, substr), '-')+1);
  *(strchr(line, '-')) = '\0';

  line[strlen(line)-1] == ' ' 
    ? line[strlen(line)-1] = '\0' 
    : line[strlen(line-1)];
  
  fprintf(out_file_buffer, "%s %s %s", line, content, tail);
};

// TODO: These are constants
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

  char *ext = strchrev(file_path, '.');

  if (strcmp(ext, ".html") != 0 && strcmp(ext, ".htm") != 0) {
    fprintf(stderr, "[kevlar] attempt to open file with invalid extension %s; can only open html files\n", file_path);
    exit(1);
  }

  char file[CONFIG_MAX_FILE_SIZE];
  char line[TEMPLATE_MAX_LINE_SIZE];

  while ((fgets(line, TEMPLATE_MAX_LINE_SIZE, file_buffer)) != NULL) {
    if (strstr(line, "--TITLE--")) {
      kevlar_write_html_string(line, "--TITLE--", kev_config->configTitle, out_file_buffer);
      continue;
    } else if (strstr(line, "--AUTHOR--")) {
      kevlar_write_html_string(line, "--AUTHOR--", kev_config->configAuthor, out_file_buffer);
      continue;
    } 
    fprintf(out_file_buffer, "%s", line);
  }

  fclose(file_buffer);
};

int main() {
  KevlarConfig config = {
    "Philip Mc revice",
    "super awesome big meat site",
    "default", 
    "default",
  };

  kevlar_parse_template("../../kevlar_theme/index.html", "../../kevlar_theme/out.html", &config);
  return 0;
}
