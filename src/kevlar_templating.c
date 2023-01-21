#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"

#include "kevlar_handle_config.h"
#include "kevlar_templating.h"
#include "kevlar_build.h"

void kevlar_parse_template_token(char line[], char substr[], char content[],
                                 FILE *out_file_buffer) {
  char tail[TEMPLATE_MAX_TAG_SIZE];
  strcpy(tail, strrchr(strstr(line, substr), '-') + 1);
  *(strchr(line, '-')) = '\0';

  line[strlen(line) - 1] == ' ' ? line[strlen(line) - 1] = '\0' : line[strlen(line - 1)];

  fprintf(out_file_buffer, "%s %s %s", line, content, tail);
};

void kevlar_parse_stylesheet(char *line, FILE *out_file_buffer, char * template_file_path) {
  char stylesheet_path[CONFIG_MAX_PATH_SIZE];
  char stylesheet[TEMPLATE_MAX_FILE_SIZE] = "";

  snprintf(stylesheet_path, CONFIG_MAX_PATH_SIZE, "%s/%s", template_file_path , strchr(line, '/') + 1);
  *(strchr(stylesheet_path, '-')) = '\0';

  FILE *css_file_buffer;
  char file_line[TEMPLATE_MAX_LINE_SIZE];

  if ((css_file_buffer = fopen(stylesheet_path, "r")) == NULL) {
    fprintf(stderr, "[kevlar] couldn't open stylesheet! Perhaps your theme "
                    "is invalid?\n");
    exit(1);
  }

  while ((fgets(file_line, TEMPLATE_MAX_LINE_SIZE, css_file_buffer)) != NULL)
    strcat(stylesheet, file_line);

  utl_prepend(stylesheet, "<style>\n");
  strcat(stylesheet, "</style>\n");
  fclose(css_file_buffer);
  kevlar_parse_template_token(line, "--STYLE", stylesheet, out_file_buffer);
};

void kevlar_parse_script(char *line, FILE *out_file_buffer, char * template_file_path) {
  char script_path[CONFIG_MAX_PATH_SIZE];
  char script[TEMPLATE_MAX_FILE_SIZE] = "";

  sprintf(script_path, "%s/%s", "template_file_path" , strchr(line, '/') + 1); 
  *(strchr(script_path, '-')) = '\0';

  FILE *js_file_buffer;
  char file_line[TEMPLATE_MAX_LINE_SIZE];

  if ((js_file_buffer = fopen(script_path, "r")) == NULL) {
    fprintf(stderr, "[kevlar] couldn't open the script! Perhaps your theme "
                    "is invalid?\n");
    exit(1);
  }

  while ((fgets(file_line, TEMPLATE_MAX_LINE_SIZE, js_file_buffer)) != NULL)
    strcat(script, file_line);

  utl_prepend(script, "<script>\n");
  strcat(script, "</script>\n");
  fclose(js_file_buffer);
  kevlar_parse_template_token(line, "--SCRIPT", script, out_file_buffer);
};

void kevlar_parse_post_from_template(char * html_file_path, char * template_file_path, char * config_file_path, ListingItem * item) {
  FILE * post_file = fopen(template_file_path, "r");
  FILE * html_file = fopen(html_file_path, "w");
  
  char line[TEMPLATE_MAX_LINE_SIZE];

  while (fgets(line, TEMPLATE_MAX_LINE_SIZE, post_file) != NULL) {
    if (strstr(line, "--DATE--")) {
      kevlar_parse_template_token(line, "--DATE--", item->lDate, html_file);
    } else if (strstr(line, "--TITLE--"))  {
      kevlar_parse_template_token(line, "--TITLE--", item->lTitle, html_file);
    } else if (strstr(line, "--CONTENT--"))  {

      puts(item->lContent);

      kevlar_parse_template_token(line, "--CONTENT--", item->lContent, html_file);
    } else if (strstr(line, "--STYLE")) {
      kevlar_parse_stylesheet(line, html_file, template_file_path);
      continue;
    } else if (strstr(line, "--SCRIPT")) {
      kevlar_parse_stylesheet(line, html_file, template_file_path);
      continue;
    } else if (strstr(line, "--")) {
      char token[CONFIG_MAX_PATH_SIZE];
      strcpy(token, strstr(line, "--")+2);
      *strstr(token, "--") = '\0';

      char opt[CONFIG_MAX_PATH_SIZE];
      kevlar_get_opt_from_config(config_file_path, token, opt);
      puts(opt);

      kevlar_parse_template_token(line, token, opt, html_file);
    } else {
      fprintf(html_file, "%s", line);
    }
  }

}
/*

void kelvar_parse_header_and_footer(FILE *out_file_buffer, KevlarConfig *kev_config) {

  // "undry" code but at least it doesn't cause the entire app to fall apart
  // due to mysterious seg faults

  FILE *header_file = fopen(kev_config->configHeaderPath, "r");

  if (header_file == NULL) {
    fprintf(stderr, "[kevlar] couldn't open %s for parsing header\n", kev_config->configHeaderPath);
    exit(1);
  }

  char line[TEMPLATE_MAX_LINE_SIZE];

  while (fgets(line, TEMPLATE_MAX_LINE_SIZE, header_file) != NULL) {

    if (strstr(line, "--TITLE--")) {
      kevlar_parse_template_token(line, "--TITLE--", kev_config->configTitle, out_file_buffer);
      continue;
    } else if (strstr(line, "--AUTHOR--")) {
      kevlar_parse_template_token(line, "--AUTHOR--", kev_config->configAuthor, out_file_buffer);
      continue;
    if (strstr(line, "--STYLE")) {
      kevlar_parse_stylesheet(line, out_file_buffer, kev_config->configTheme);
      continue;
    } else if (strstr(line, "--SCRIPT")) {
      kevlar_parse_stylesheet(line, out_file_buffer, kev_config->configTheme);
      continue;
    } else {
      fprintf(out_file_buffer, "%s", line);
    }
  }

  fclose(header_file);
}

void kevlar_parse_template(FILE *in_file_buffer, FILE *out_file_buffer, KevlarConfig *kev_config) {
  char line[TEMPLATE_MAX_LINE_SIZE];

  while ((fgets(line, TEMPLATE_MAX_LINE_SIZE, in_file_buffer)) != NULL) {
    if (strstr(line, "--TITLE--")) {
      kevlar_parse_template_token(line, "--TITLE--", kev_config->configTitle, out_file_buffer);
      continue;
    } else if (strstr(line, "--AUTHOR--")) {
      kevlar_parse_template_token(line, "--AUTHOR--", kev_config->configAuthor, out_file_buffer);
      continue;
    } else if (strstr(line, "--LISTING--")) {
      kevlar_parse_template_token(line, "--LISTING--", kev_config->configListing, out_file_buffer);
      continue;
    } else if (strstr(line, "--STYLE")) {
      kevlar_parse_stylesheet(line, out_file_buffer, kev_config->configTheme);
      continue;
    } else if (strstr(line, "--SCRIPT")) {
      kevlar_parse_script(line, out_file_buffer, kev_config->configTheme);
      continue;
    } else if (strstr(line, "--HEADER--")) {
      kelvar_parse_header_and_footer(out_file_buffer, kev_config);
      continue;
    } else if (strstr(line, "--FOOTER--")) {
      kelvar_parse_header_and_footer(out_file_buffer, kev_config);
      continue;
    } else if (strstr(line, "--CONTENT--")) {
      kevlar_parse_template_token(line, "--CONTENT--", kev_config->configHtmlContents,
                                  out_file_buffer);
      continue;
    } else {
      fprintf(out_file_buffer, "%s", line);
    }
  }
};

void kevlar_build_template(char *in_file_path, char *out_file_path, KevlarConfig *kev_config) {
  FILE *infile = fopen(in_file_path, "r");
  FILE *outfile = fopen(out_file_path, "w");

  if (infile == NULL) {
    fprintf(stderr, "[kevlar] couldn't find %s while building the template\n", in_file_path);
    exit(1);
  }

  kevlar_parse_template(infile, outfile, kev_config);

  fclose(infile);
  fclose(outfile);
}
*/
