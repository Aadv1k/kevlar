#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "./kevlar_rst_to_html.h"
#include "kevlar_errors.h"

FILE *rst_infile;
FILE *rst_outfile;

long rst_get_file_length(char *filename) {
  FILE *infile = fopen(filename, "r");

  long file_len = 0;

  for (char c = getc(infile); c != EOF; c = getc(infile)) {
    if (c == '\n')
      file_len++;
  }

  return file_len;
}

void rst_throw_error(const char *message, int line, char context[][RST_LINE_LENGTH]) {
  printf("[rst2html]: %s \n%d | %s\n>%d| %s\n%d | %s\n", message, line, context[line - 1], line + 1,
         context[line], line + 2, context[line + 1]);
  exit(1);
}

int rst_is_para_only(char file[][RST_LINE_LENGTH], int line) {
  if (strlen(file[line]) == 0)
    return 0;
  // TODO: This is pretty messy and might cause troubles later on
  if ((file[line + 1][0] != '-' && file[line - 1][0] != '-') &&
      (file[line + 1][0] != '=' && file[line - 1][0] != '=') &&
      (file[line + 1][0] != '#' && file[line - 1][0] != '#') &&
      (!isdigit(file[line + 1][0]) && !isdigit(file[line - 1][0])) &&
      (file[line + 1][0] != ' ' && file[line - 1][0] != ' ')) {
    return 1;
  }
  return 0;
}

void rst_handle_equal(char file[][RST_LINE_LENGTH], int line) {

  static bool equalOpen = false;

  if (equalOpen) {
    equalOpen = false;
    return;
  }

  if (strcmp(file[line + 2], file[line]) == 0 && strlen(file[line]) == strlen(file[line + 2]) &&
      equalOpen == false) {

    fprintf(rst_outfile, "<h1>%s</h1>\n", file[line + 1]);
    equalOpen = true;

  } else if (strlen(file[line - 1]) == strlen(file[line]) && strcmp(file[line - 2], file[line])) {
    fprintf(rst_outfile, "\n<h2>%s</h2>\n", file[line - 1]);
  } else {
    rst_throw_error("Error while parsing title", line, file);
  }
}

void rst_handleText(char input[RST_LINE_LENGTH], char output[RST_LINE_LENGTH]) {
  bool boldOpen = true;
  bool emOpen = true;

  for (int i = 0; input[i] != '\0'; i++) {
    if (input[i] == '*' && input[i + 1] != '*' && input[i + 2] != '*') {
      emOpen ? strcat(output, "<em>") : strcat(output, "</em>");
      emOpen = !emOpen;
    } else if (input[i] == '*' && input[i + 1] == '*' && input[i + 2] != '*') {
      boldOpen ? strcat(output, "<b>") : strcat(output, "</b>");
      boldOpen = !boldOpen;
      i += 1;
    } else if (input[i] == '*' && input[i + 1] == '*' && input[i + 2] == '*') {
      (boldOpen && emOpen) ? strcat(output, "<em><b>") : strcat(output, "</b></em>");
      boldOpen = !boldOpen;
      emOpen = !emOpen;
      i += 2;
    } else {
      strncat(output, &input[i], 1);
    }
  }
}

void rst_handle_para(char file[][RST_LINE_LENGTH], int line) {
  char out[RST_LINE_LENGTH];
  rst_handleText(file[line], out);
  fprintf(rst_outfile, "\n<p>%s</p>\n", out);
}

void rst_handle_number(char file[][RST_LINE_LENGTH], int line) {
  static bool olListOpenNum = false;
  char out[RST_LINE_LENGTH];
  rst_handleText(file[line], out);

  if (file[line][1] == '.' && olListOpenNum == 0 && strlen(file[line - 1]) == 0) {
    olListOpenNum = true;

    fprintf(rst_outfile, "<ol>\n\t<li>\n\t\t");
    fprintf(rst_outfile, "%s", out);
    fprintf(rst_outfile, "\n\t</li>\n");

  } else if (file[line][1] == '.' && olListOpenNum == 1 && strlen(file[line + 1]) != 0) {

    // This is the middle of the list, we don't need to surround by a <ul>
    // or </ul>

    fprintf(rst_outfile, "\t<li>\n\t");
    fprintf(rst_outfile, "%s", out);
    fprintf(rst_outfile, "</li>\n");

  } else if (file[line][1] == '.' && olListOpenNum == 1 && !isdigit(file[line + 1][0])) {

    // With this setup, only top level lists are supported, no nesting
    // unfortunately End of the list, we append a </ul> at the end to close
    // the list

    olListOpenNum = false;

    fprintf(rst_outfile, "\t<li>\n\t\t");
    fprintf(rst_outfile, "%s", out);
    fprintf(rst_outfile, "\n\t</li>\n</ol>\n\n");

  } else {
    // rst_throw_error("Error while parsing list item or h3", line, file);
  }
}

void rst_handle_hash(char file[][RST_LINE_LENGTH], int line) {
  static bool olListOpen = false;
  char out[RST_LINE_LENGTH];
  rst_handleText(file[line], out);

  if (file[line][1] == '.' && olListOpen == 0 && strlen(file[line - 1]) == 0) {
    olListOpen = true;

    fprintf(rst_outfile, "<ol>\n\t<li>\n\t\t");
    fprintf(rst_outfile, "%s", out);
    fprintf(rst_outfile, "\n\t</li>\n");

  } else if (file[line][1] == '.' && olListOpen == 1 && strlen(file[line + 1]) != 0) {

    // This is the middle of the list, we don't need to surround by a <ul>
    // or </ul>

    fprintf(rst_outfile, "\t<li>\n\t");
    fprintf(rst_outfile, "%s", out);
    fprintf(rst_outfile, "</li>\n");

  } else if (file[line][1] == '.' && olListOpen == 1 && file[line + 1][0] != '#') {

    // With this setup, only top level lists are supported, no nesting
    // unfortunately End of the list, we append a </ul> at the end to close
    // the list

    olListOpen = false;

    fprintf(rst_outfile, "\t<li>\n\t\t");
    fprintf(rst_outfile, "%s", out);
    fprintf(rst_outfile, "\n\t</li>\n</ol>\n\n");
  } else {
    // Ignore this string, we asume it is a comment
  }
}

void rst_handle_dash_and_ul(char file[][RST_LINE_LENGTH], int line) {
  static bool listOpen = true;

  char out[RST_LINE_LENGTH] = "";
  rst_handleText(file[line], out);

  if (strlen(file[line]) == strlen(file[line - 1])) {
    // It a header with dashed underline or H3
    fprintf(rst_outfile, "\n<h3>%s</h3>\n", file[line - 1]);

  } else if (file[line][1] == ' ' && listOpen == 0 && strlen(file[line - 1]) == 0) {

    // Start of the list, we append a <ul> at the beginning

    listOpen = true;

    fprintf(rst_outfile, "<ul>\n\t<li>\n\t\t%s", &out[1]);
    fprintf(rst_outfile, "\n\t</li>\n");

  } else if (file[line][1] == ' ' && listOpen == 1 && strlen(file[line + 1]) != 0) {

    // This is the middle of the list, we don't need to surround by a <ul>
    // or </ul>

    fprintf(rst_outfile, "\t<li>\n\t%s", &out[1]);
    fprintf(rst_outfile, "</li>\n");

  } else if (file[line][1] == ' ' && listOpen == 1 && file[line + 1][0] != '-') {

    // With this setup, only top level lists are supported, no nesting
    // unfortunately End of the list, we append a </ul> at the end to close
    // the list

    listOpen = false;

    fprintf(rst_outfile, "\t<li>\n\t\t%s", out);
    fprintf(rst_outfile, "\n\t</li>\n</ul>\n\n");

  } else {
    // rst_throw_error("Error while parsing unordered list item or h3", line, file);
  }
}

void rst_handle_fieldlist(char file[][RST_LINE_LENGTH], int line) {
  char param[RST_FIELD_LIST_SIZE] = "";
  char opt[RST_FIELD_LIST_SIZE] = "";
  bool open = false;

  for (int i = 0, k = 0; file[line][i] != '\0'; i++, k++) {
    char cur = file[line][i];
    if (cur == ':' && i == 0 && open != 1) {
      open = true;
    } else if (cur == ':' && open == 1) {
      open = false;
      for (int j = i + 1; file[line][j] != '\0'; j++) {
        strncat(opt, &file[line][j], 1);
      }
    } else if (open) {
      strncat(param, &cur, 1);
    }
  }

  if (open || strlen(param) == 0 || strlen(opt) < 1 || opt[0] != ' ') {
    printf("RST Line %d: PARAM OR OPT FORMAT NOT CORRET\n", line + 1);
    return;
  };
};

void usage() {
  puts("kev_rst2html -[OPT] INPUT.rst OUTPUT.html\n\t-h -- On invalid rst "
       "exit "
       "with error message\n");
  exit(1);
}

void rst_parse(char *rst_file_path, char *html_file_path) {
  if (strcmp(strrchr(rst_file_path, '.'), ".rst") != 0) {
    kevlar_err("[%s] %s doesn't seem to be a rst file", __FILE__, rst_file_path);
  }

  rst_infile = fopen(rst_file_path, "r");
  rst_outfile = fopen(html_file_path, "w");

  if (rst_infile == NULL) {
    kevlar_err("[%s] the file \"%s\" doesn't exist.", __FILE__, rst_file_path);
  }

  long fileLength = rst_get_file_length(rst_file_path);
  char file[fileLength][RST_LINE_LENGTH];

  // Read contents of the input file into the file[]
  for (int i = 0; !feof(rst_infile); i++) {
    if (fgets(file[i], RST_LINE_LENGTH, rst_infile) == NULL) // pass;
    utl_truncateLast(file[i]);
  }


  for (long currentLine = 0; currentLine < fileLength; currentLine++) {
    switch (file[currentLine][0]) {
    case '=':
      rst_handle_equal(file, currentLine);
      break;
    case '-':
      rst_handle_dash_and_ul(file, currentLine);
      break;
    case ':':
      rst_handle_fieldlist(file, currentLine);
      break;
    case '#':
      rst_handle_hash(file, currentLine);
      break;
    case '\n':
    case '\r':
    case ' ':
      break;
    default:
      if (isdigit(file[currentLine][1])) {
        rst_handle_number(file, currentLine);
        break;
      }

      if (rst_is_para_only(file, currentLine) == 1) {
        rst_handle_para(file, currentLine);
      }
    }
  }

  fclose(rst_infile);
  fclose(rst_outfile);
}
