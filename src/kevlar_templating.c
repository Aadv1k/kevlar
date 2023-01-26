#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"

#include "kevlar_build.h"
#include "kevlar_errors.h"
#include "kevlar_handle_config.h"
#include "kevlar_templating.h"

void kevlar_parse_template_token(char line[], char substr[], char content[],
                                 FILE *out_file_buffer) {
  char tail[TEMPLATE_MAX_TAG_SIZE];
  strcpy(tail, strrchr(strstr(line, substr), '-') + 1);
  *(strchr(line, '-')) = '\0';

  line[strlen(line) - 1] == ' ' ? line[strlen(line) - 1] = '\0' : line[strlen(line - 1)];

  fprintf(out_file_buffer, "%s %s %s", line, content, tail);
};

void kevlar_parse_template_str_token(char line[], char substr[], char content[], char *target) {
  char tail[TEMPLATE_MAX_TAG_SIZE];
  strcpy(tail, strrchr(strstr(line, substr), '-') + 1);
  *(strchr(line, '-')) = '\0';

  line[strlen(line) - 1] == ' ' ? line[strlen(line) - 1] = '\0' : line[strlen(line - 1)];

  char cur[CONFIG_MAX_FILE_SIZE];
  sprintf(cur, "%s %s %s", line, content, tail);
  strcat(target, cur);
};

void kevlar_parse_inline_tag(char *line, char *template_file_path, char *tag, FILE *out_buf) {
  char stylesheet_path[CONFIG_MAX_PATH_SIZE];
  char stylesheet[TEMPLATE_MAX_FILE_SIZE] = "";

  snprintf(stylesheet_path, CONFIG_MAX_PATH_SIZE, "%s/%s", template_file_path,
           strchr(line, '/') + 1);
  utl_truncateLast(stylesheet_path);
  utl_truncateLast(stylesheet_path);
  utl_truncateLast(stylesheet_path);

  FILE *css_file_buffer;
  char file_line[TEMPLATE_MAX_LINE_SIZE];

  if ((css_file_buffer = fopen(stylesheet_path, "r")) == NULL) {
    fprintf(stderr, "[kevlar] couldn't open stylesheet! Perhaps your theme "
                    "is invalid?\n");
    exit(1);
  }

  while ((fgets(file_line, TEMPLATE_MAX_LINE_SIZE, css_file_buffer)) != NULL)
    strcat(stylesheet, file_line);

  char parsed_tag[TEMPLATE_MAX_TAG_SIZE];
  sprintf(parsed_tag, "<%s>\n", tag);

  char parsed_tag_close[TEMPLATE_MAX_TAG_SIZE];
  sprintf(parsed_tag_close, "</%s>\n", tag);

  utl_prepend(stylesheet, parsed_tag);
  strcat(stylesheet, parsed_tag_close);
  fclose(css_file_buffer);

  if (strcmp(tag, "style") == 0) {
    kevlar_parse_template_token(line, "--STYLE", stylesheet, out_buf);
  } else {
    kevlar_parse_template_token(line, "--SCRIPT", stylesheet, out_buf);
  }
};

void kevlar_parse_str_inline_tag(char *line, char *template_file_path, char *tag, char *output) {
  char stylesheet_path[CONFIG_MAX_PATH_SIZE];
  char stylesheet[TEMPLATE_MAX_FILE_SIZE] = "";

  snprintf(stylesheet_path, CONFIG_MAX_PATH_SIZE, "%s/%s", template_file_path,
           strchr(line, '/') + 1);
  utl_truncateLast(stylesheet_path);
  utl_truncateLast(stylesheet_path);
  utl_truncateLast(stylesheet_path);

  FILE *css_file_buffer;
  char file_line[TEMPLATE_MAX_LINE_SIZE];

  if ((css_file_buffer = fopen(stylesheet_path, "r")) == NULL) {
    fprintf(stderr, "[kevlar] couldn't open stylesheet! Perhaps your theme "
                    "is invalid?\n");
    exit(1);
  }

  while ((fgets(file_line, TEMPLATE_MAX_LINE_SIZE, css_file_buffer)) != NULL)
    strcat(stylesheet, file_line);

  char parsed_tag[TEMPLATE_MAX_TAG_SIZE];
  sprintf(parsed_tag, "<%s>\n", tag);

  char parsed_tag_close[TEMPLATE_MAX_TAG_SIZE];
  sprintf(parsed_tag_close, "</%s>\n", tag);

  utl_prepend(stylesheet, parsed_tag);
  strcat(stylesheet, parsed_tag_close);
  fclose(css_file_buffer);

  if (strcmp(tag, "style") == 0) {
    kevlar_parse_template_str_token(line, "--STYLE", stylesheet, output);
  } else {
    kevlar_parse_template_str_token(line, "--SCRIPT", stylesheet, output);
  }
};

void kevlar_parse_header_and_footer(FILE *header_buf, char *output, char *template_folder,
                                    char *config_file_path) {
  char line[TEMPLATE_MAX_LINE_SIZE];
  while (fgets(line, TEMPLATE_MAX_LINE_SIZE, header_buf) != NULL) {
    if (strstr(line, "--STYLE")) {
      kevlar_parse_str_inline_tag(line, template_folder, "style", output);
      continue;
    } else if (strstr(line, "--SCRIPT")) {
      kevlar_parse_str_inline_tag(line, template_folder, "script", output);
      continue;
    } else if (strstr(line, "--")) {
      char token[CONFIG_MAX_PATH_SIZE];
      strcpy(token, strstr(line, "--") + 2);
      *strstr(token, "--") = '\0';
      char opt[CONFIG_MAX_PATH_SIZE];
      kevlar_get_opt_from_config(config_file_path, token, opt);
      kevlar_parse_template_str_token(line, token, opt, output);
      continue;
    } else {
      strcat(output, line);
    }
  }
}

void kevlar_parse_header(FILE *output_buf, char *template_folder, char *config_file_path) {
  char header_path[CONFIG_MAX_PATH_SIZE];
  strcpy(header_path, template_folder);
  strcat(header_path, "/header.html");
  char output[CONFIG_MAX_FILE_SIZE] = "";
  FILE *header_buf = fopen(header_path, "r");
  if (!header_buf)
    kevlar_err("couldn't open %s perhaps your theme is invalid", header_path);
  kevlar_parse_header_and_footer(header_buf, output, template_folder, config_file_path);
  fprintf(output_buf, "%s", output);
}

void kevlar_parse_footer(FILE *output_buf, char *template_folder, char *config_file_path) {
  char footer_path[CONFIG_MAX_PATH_SIZE];
  strcpy(footer_path, template_folder);
  strcat(footer_path, "/footer.html");
  char output[CONFIG_MAX_FILE_SIZE] = "";
  FILE *footer_buf = fopen(footer_path, "r");
  puts(footer_path);
  if (!footer_buf)
    kevlar_err("couldn't open %s perhaps your theme is invalid", footer_path);
  kevlar_parse_header_and_footer(footer_buf, output, template_folder, config_file_path);
  fprintf(output_buf, "%s", output);
}

void kevlar_generate_listing(char *entry_template_file_path, char *config_file_path,
                             ListingItem *itemsList, char *target, int size) {
  target[0] = '\0';
  FILE *template = fopen(entry_template_file_path, "r");
  if (!template)
    kevlar_err("couldn't open %s Perhaps your theme is invalid?", entry_template_file_path);

  char line[TEMPLATE_MAX_LINE_SIZE];

  printf("%d\n", size);
  for (int i = 0; i < size; i++) {
    while (fgets(line, TEMPLATE_MAX_LINE_SIZE, template)) {
      if (strstr(line, "--DATE--")) {
        kevlar_parse_template_str_token(line, "--DATE--", itemsList[i].lDate, target);
      } else if (strstr(line, "--TITLE--")) {
        kevlar_parse_template_str_token(line, "--TITLE--", itemsList[i].lTitle, target);
      } else if (strstr(line, "--PATH--")) {
        kevlar_parse_template_str_token(line, "--PATH--", strrchr(itemsList[i].lPath, '/')+1, target);
      } else if (strstr(line, "--CONTENT--")) {
        kevlar_parse_template_str_token(line, "--CONTENT--", itemsList[i].lContent, target);
      } else if (strstr(line, "--")) {
        char token[CONFIG_MAX_PATH_SIZE];
        strcpy(token, strstr(line, "--") + 2);
        *strstr(token, "--") = '\0';
        char opt[CONFIG_MAX_PATH_SIZE];
        kevlar_get_opt_from_config(config_file_path, token, opt);
        kevlar_parse_template_str_token(line, token, opt, target);
        continue;
      } else {
        strcat(target, line);
      }
    }
    rewind(template);
  }
  fclose(template);
}

void kevlar_parse_post_from_template(char *html_file_path, char *template_file_path,
                                     char *config_file_path, ListingItem * item) {
  FILE *post_file = fopen(template_file_path, "r");

  if (post_file == NULL) {
    kevlar_err("couldn't open %s perhaps your theme is invalid", template_file_path);
  }

  FILE *html_file = fopen(html_file_path, "w");

  char *listing = malloc(CONFIG_MAX_FILE_SIZE * sizeof(char));

  char *template_folder = strdup(template_file_path);
  *strrchr(template_folder, '/') = '\0';

  char entry_template_path[CONFIG_MAX_PATH_SIZE];
  strcpy(entry_template_path, template_folder);
  strcat(entry_template_path, "/entry.html");
  

  char line[TEMPLATE_MAX_LINE_SIZE];

  while (fgets(line, TEMPLATE_MAX_LINE_SIZE, post_file) != NULL) {
    if (strstr(line, "--DATE--")) {
      kevlar_parse_template_token(line, "--DATE--", item->lDate, html_file);
    } else if (strstr(line, "--TITLE--")) {
      kevlar_parse_template_token(line, "--TITLE--", item->lTitle, html_file);
    } else if (strstr(line, "--HEADER--") || strstr(line, "--FOOTER--")) {
      kevlar_parse_header(html_file, template_folder, config_file_path);
    } else if (strstr(line, "--CONTENT--")) {
      kevlar_parse_template_token(line, "--CONTENT--", item->lContent, html_file);
    } else if (strstr(line, "--STYLE")) {
      kevlar_parse_inline_tag(line, template_folder, "style", html_file);
      continue;
    } else if (strstr(line, "--SCRIPT")) {
      kevlar_parse_inline_tag(line, template_folder, "script", html_file);
      continue;
    } else if (strstr(line, "--")) {
      char token[CONFIG_MAX_PATH_SIZE];
      strcpy(token, strstr(line, "--") + 2);
      *strstr(token, "--") = '\0';
      char opt[CONFIG_MAX_PATH_SIZE];
      kevlar_get_opt_from_config(config_file_path, token, opt);
      kevlar_parse_template_token(line, token, opt, html_file);
      continue;
    } else {
      fprintf(html_file, "%s", line);
    }
  }

  free(template_folder);
  free(listing);
}

void kevlar_generate_index_from_template(char *dist_path, char *template_folder_path,
                                         char *config_path, ListingItem *itemsList, int size) {
  char entry_template_path[CONFIG_MAX_PATH_SIZE];
  char index_template_path[CONFIG_MAX_PATH_SIZE];

  strcpy(entry_template_path, template_folder_path);
  strcpy(index_template_path, template_folder_path);

  strcat(entry_template_path, "/entry.html");
  strcat(index_template_path, "/index.html");

  char *output = malloc(CONFIG_MAX_FILE_SIZE * sizeof(char));
  kevlar_generate_listing(entry_template_path, config_path, itemsList, output, size);

  FILE *template = fopen(index_template_path, "r");
  FILE *html_file = fopen(dist_path, "w");

  char line[TEMPLATE_MAX_LINE_SIZE];

  while (fgets(line, TEMPLATE_MAX_LINE_SIZE, template) != NULL) {
    if (strstr(line, "--STYLE")) {
      kevlar_parse_inline_tag(line, template_folder_path, "style", html_file);
      continue;
    } else if (strstr(line, "--LISTING--")) {
      kevlar_parse_template_token(line, "--LISTING--", output, html_file);
    } else if (strstr(line, "--SCRIPT")) {
      kevlar_parse_inline_tag(line, template_folder_path, "script", html_file);
      continue;
    } else if (strstr(line, "--HEADER--")) {
      kevlar_parse_header(html_file, template_folder_path, config_path);
    } else if (strstr(line, "--FOOTER--")) {
      kevlar_parse_footer(html_file, template_folder_path, config_path);
    } else if (strstr(line, "--")) {
      char token[CONFIG_MAX_PATH_SIZE];
      strcpy(token, strstr(line, "--") + 2);
      *strstr(token, "--") = '\0';
      char opt[CONFIG_MAX_PATH_SIZE];
      kevlar_get_opt_from_config(config_path, token, opt);
      kevlar_parse_template_token(line, token, opt, html_file);
      continue;
    } else {
      fprintf(html_file, "%s", line);
    }
  }

  fclose(html_file);
  fclose(template);
  free(output);
}
