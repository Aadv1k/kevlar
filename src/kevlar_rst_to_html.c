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

char * rst_handleText(char file[][RST_LINE_LENGTH], int line) {
  bool open = true;
  int style = 0;
  char res[RST_LINE_LENGTH] = ""; 



  char content[strlen(file[line])];
  strcpy(content, file[line]);
  
  // Brace yourselves for whats about to come, it is NOT pretty

  bool tickOpen = false;
  char backTickContent[RST_LINE_LENGTH];

  for (int i = 0; content[i] != '\0'; i++) {
    // If we find an asterisk, we add to the style, style can keep track of
    // how many astersiks we have

    if (tickOpen == true) {
      strncat(backTickContent, &content[i], 1);
    }

    if (content[i] == '`' && tickOpen == false) {

      tickOpen = true;

    } else if (content[i] == '_' && content[i - 1] == '`' && tickOpen == true) {

      utl_truncateLast(backTickContent);
      char html_link_tag[RST_LINE_LENGTH];
      char html_link_name[RST_LINE_LENGTH];

      char *html_link = strrchr(backTickContent, ' ');
      utl_truncateLast(html_link);

      strcpy(html_link_name, backTickContent);

      *(strrchr(html_link_name, ' ')) = '\0';

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
      snprintf(html_link_tag, RST_LINE_LENGTH * 2, "<a href=\"%s\">%s</a>", html_link + 1,
               html_link_name);
#pragma GCC diagnostic pop
      strcat(res, html_link_tag);

      backTickContent[0] = '\0';
      tickOpen = false;
      continue;

    } else if (content[i] != '_' && content[i - 1] == '`' && tickOpen == true) {
      char code_line[RST_LINE_LENGTH];

      utl_truncateLast(backTickContent);
      utl_truncateLast(backTickContent);


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
      snprintf(code_line, RST_LINE_LENGTH + 20, "<code>%s</code> ", backTickContent);
#pragma GCC diagnostic pop
      strcat(res, code_line);

      backTickContent[0] = '\0';
      tickOpen = false;

      continue;
    }

    if (content[i] == '*') {
      style++;

      // If there is some content after the asterisk and we put an opening
      // tag
      if (content[i + 1] != '*' && open == true) {
        switch (style) {
        case 1:
          strcat(res, "<em>");
          break;
        case 2:
          strcat(res, "<b>");
          break;
        case 3:
          strcat(res, "<em><b>");
          break;
        }
        // We have put a need to add a closing tag
        open = false;
      } else if (content[i + 1] != '*' && open == false) {
        switch (style) {
        case 1:
          strcat(res, "</em>");
          break;
        case 2:
          strcat(res, "</b>");
          break;
        case 3:
          strcat(res, "</b></em>");
          break;
        }
        // we default the open = true for the next asterisk.
        open = true;
      }
    } else if (content[i] != '*' && tickOpen == false) {
      // We reset the style to 0 otherwise it will keep on adding,
      // ideally, we can extend this setup infinitely given how we can
      // keep track of how many asterisks we have
      strncat(res, &content[i], 1);
      style = 0;
    }
  }

  char * choppedLine;

  if ((res[0] == '-' && res[1] == ' ') || (res[0] == '#' && res[1] == '.') ||
      (isdigit(res[0]) && res[1] == '.')) {
    choppedLine = res + 2;
  } else {
    choppedLine = res;
  }

  return choppedLine;
}

void rst_handle_para(char file[][RST_LINE_LENGTH], int line) {
  char * foo; 
  foo = rst_handleText(file, line);
  fprintf(rst_outfile, "\n<p>%s</p>\n", foo);
}

void rst_handle_number(char file[][RST_LINE_LENGTH], int line) {
  static bool olListOpenNum = false;

  if (file[line][1] == '.' && olListOpenNum == 0 && strlen(file[line - 1]) == 0) {
    olListOpenNum = true;

    fprintf(rst_outfile, "<ol>\n\t<li>\n\t\t");
    fprintf(rst_outfile, "%s", rst_handleText(file, line));
    fprintf(rst_outfile, "\n\t</li>\n");

  } else if (file[line][1] == '.' && olListOpenNum == 1 && strlen(file[line + 1]) != 0) {

    // This is the middle of the list, we don't need to surround by a <ul>
    // or </ul>

    fprintf(rst_outfile, "\t<li>\n\t");
    fprintf(rst_outfile, "%s", rst_handleText(file, line));
    fprintf(rst_outfile, "</li>\n");

  } else if (file[line][1] == '.' && olListOpenNum == 1 && !isdigit(file[line + 1][0])) {

    // With this setup, only top level lists are supported, no nesting
    // unfortunately End of the list, we append a </ul> at the end to close
    // the list

    olListOpenNum = false;

    fprintf(rst_outfile, "\t<li>\n\t\t");
    fprintf(rst_outfile, "%s", rst_handleText(file, line));
    fprintf(rst_outfile, "\n\t</li>\n</ol>\n\n");

  } else {
    // rst_throw_error("Error while parsing list item or h3", line, file);
  }
}

void rst_handle_hash(char file[][RST_LINE_LENGTH], int line) {
  static bool olListOpen = false;

  if (file[line][1] == '.' && olListOpen == 0 && strlen(file[line - 1]) == 0) {
    olListOpen = true;

    fprintf(rst_outfile, "<ol>\n\t<li>\n\t\t");
    fprintf(rst_outfile, "%s", rst_handleText(file, line));
    fprintf(rst_outfile, "\n\t</li>\n");

  } else if (file[line][1] == '.' && olListOpen == 1 && strlen(file[line + 1]) != 0) {

    // This is the middle of the list, we don't need to surround by a <ul>
    // or </ul>

    fprintf(rst_outfile, "\t<li>\n\t");
    fprintf(rst_outfile, "%s", rst_handleText(file, line));
    fprintf(rst_outfile, "</li>\n");

  } else if (file[line][1] == '.' && olListOpen == 1 && file[line + 1][0] != '#') {

    // With this setup, only top level lists are supported, no nesting
    // unfortunately End of the list, we append a </ul> at the end to close
    // the list

    olListOpen = false;

    fprintf(rst_outfile, "\t<li>\n\t\t");
    fprintf(rst_outfile, "%s", rst_handleText(file, line));
    fprintf(rst_outfile, "\n\t</li>\n</ol>\n\n");
  } else {
    // Ignore this string, we asume it is a comment
  }
}

void rst_handle_dash_and_ul(char file[][RST_LINE_LENGTH], int line) {
  static bool listOpen = false;

  if (strlen(file[line]) == strlen(file[line - 1])) {

    // It a header with dashed underline or H3
    fprintf(rst_outfile, "\n<h3>%s</h3>\n", file[line - 1]);

  } else if (file[line][1] == ' ' && listOpen == 0 && strlen(file[line - 1]) == 0) {

    // Start of the list, we append a <ul> at the beginning

    listOpen = true;

    fprintf(rst_outfile, "<ul>\n\t<li>\n\t\t%s", rst_handleText(file, line));
    fprintf(rst_outfile, "\n\t</li>\n");

  } else if (file[line][1] == ' ' && listOpen == 1 && strlen(file[line + 1]) != 0) {

    // This is the middle of the list, we don't need to surround by a <ul>
    // or </ul>

    fprintf(rst_outfile, "\t<li>\n\t%s", rst_handleText(file, line));
    fprintf(rst_outfile, "</li>\n");

  } else if (file[line][1] == ' ' && listOpen == 1 && file[line + 1][0] != '-') {

    // With this setup, only top level lists are supported, no nesting
    // unfortunately End of the list, we append a </ul> at the end to close
    // the list

    listOpen = false;

    fprintf(rst_outfile, "\t<li>\n\t\t%s", rst_handleText(file, line));
    fprintf(rst_outfile, "\n\t</li>\n</ul>\n\n");
  } else {
    // rst_throw_error("Error while parsing unordered list item or h3",
    // line, file);
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
