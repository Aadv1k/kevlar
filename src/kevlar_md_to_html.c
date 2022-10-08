#include "kevlar_md_to_html.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "kevlar_errors.h"
#include "kevlar_handle_config.h"
#include "kevlar_rst_to_html.h"

FILE *md_infile;
FILE *md_outfile;

bool md_is_hr(char file[][RST_LINE_LENGTH], int line) {
  if (strlen(file[line]) != MD_HR_LENGTH) {
    return false;
  }

  for (int i = 0; i < 3; i++) {
    if (file[line][i] != file[line][0])
      return false;
  }

  return true;
}

bool md_is_single_para(char file[][RST_LINE_LENGTH], int line) {
  // This is the simplest method to ensure/control what constitues a new line
  // para or a grouped para

  if ((file[line + 1][0] == '-' || file[line + 1][0] == '*') &&
      (file[line - 1][0] == '-' || file[line - 1][0] == '*') &&
      (isdigit(file[line - 1][0]) || isdigit(file[line + 1][0]))) {
    return false;
  }

  if ((isspace(*file[line + 1]) != 0 || strlen(file[line + 1]) == 0) &&
      (isspace(*file[line - 1]) != 0 || strlen(file[line - 1]) == 0))
    return true;

  return false;
}

void md_handleText(char input[RST_LINE_LENGTH], char output[RST_LINE_LENGTH]) {
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

void md_handle_heading(char file[][RST_LINE_LENGTH], int line) {
  int hashCount = 0;

  while (true) {
    if (file[line][hashCount] != '#')
      break;
    hashCount++;
  }

  if (hashCount < MD_HEADING_LEVEL) {
    fprintf(md_outfile, "<h%d>%s</h%d>\n", hashCount,
            isspace(*(strrchr(file[line], '#') + 1)) ? strrchr(file[line], '#') + 2
                                                     : strrchr(file[line], '#') + 1,
            hashCount);
  } else {
    fprintf(md_outfile, "<h6>%s</h6>\n", strrchr(file[line], '#') + 1);
  }
}

void md_handle_list(char file[][RST_LINE_LENGTH], int line) {
  static bool ulOpen = false;
  char *target_line = isspace(file[line][2]) ? &file[line][3] : &file[line][2];
  char target[RST_LINE_LENGTH] = "";
  md_handleText(target_line, target);

  if (file[line - 1][0] != file[line][0] && ulOpen == false) {
    fprintf(md_outfile, "<ul>\n\t<li>%s</li>\n", target);
    ulOpen = true;
  } else if (ulOpen && file[line + 1][0] == file[line][0]) {
    fprintf(md_outfile, "\t<li>%s</li>\n", target);
  } else if (ulOpen && file[line + 1][0] != file[line][0]) {
    fprintf(md_outfile, "\t<li>%s</li>\n</ul>\n", target);
    ulOpen = false;
  }
}

void md_handle_numbered_list(char file[][RST_LINE_LENGTH], int line) {
  static bool olOpen = false;
  char *target_line = isspace(file[line][2]) ? &file[line][3] : &file[line][2];
  char target[RST_LINE_LENGTH] = "";
  md_handleText(target_line, target);

  if (file[line - 1][0] != file[line][0] && !olOpen) {
    fprintf(md_outfile, "<ol>\n\t<li>%s</li>\n", target);
    olOpen = true;
  } else if (olOpen && file[line + 1][0] == file[line][0]) {
    fprintf(md_outfile, "\t<li>%s</li>\n", target);
  } else if (olOpen && file[line + 1][0] != file[line][0]) {
    fprintf(md_outfile, "\t<li>%s</li>\n</ol>\n", target);
    olOpen = false;
  }
}

void md_handle_single_para(char file[][RST_LINE_LENGTH], int line) {
  char parsed_output[RST_LINE_LENGTH] = "";
  md_handleText(file[line], parsed_output);

  fprintf(md_outfile, "<p>%s</p>\n", parsed_output);
}

static bool paraOpen = false;

void md_handle_para(char file[][RST_LINE_LENGTH], int line) {
  char parsed_output[RST_LINE_LENGTH] = "";
  md_handleText(file[line], parsed_output);

  if (paraOpen == false) {
    fprintf(md_outfile, "<p>%s", parsed_output);
    paraOpen = true;
  } else if (paraOpen && strlen(file[line + 1]) > 1) {
    fprintf(md_outfile, " %s", parsed_output);
  } else if (paraOpen && strlen(file[line + 1]) <= 2) {
    fprintf(md_outfile, " %s</p>\n", parsed_output);
    paraOpen = false;
  }
}

void md_force_close_para() {
  fprintf(md_outfile, "</p>\n");
  paraOpen = false;
}

void md_parse(char *in_file_path, char *out_file_path) {
  if (strcmp(strrchr(in_file_path, '.'), ".md") != 0) {
    kevlar_err("[%s] %s doesn't seem to be a markdown file\n", __FILE__, in_file_path);
  }

  if ((md_infile = fopen(in_file_path, "r")) == NULL) {
    kevlar_err("[%s] the file \"%s\" doesn't exist.", __FILE__, in_file_path);
  }

  md_outfile = fopen(out_file_path, "w");
  long fileLength = rst_get_file_length(in_file_path);
  char file[fileLength][RST_LINE_LENGTH];

  for (int i = 0; i < fileLength; i++) {
    fgets(file[i], RST_LINE_LENGTH, md_infile);
    utl_truncateLast(file[i]);
  }

  for (long currentLine = 0; currentLine < fileLength; currentLine++) {
    switch (file[currentLine][0]) {
    case '#':
      md_handle_heading(file, currentLine);
      break;
    case '*':
    case '-':
      if (md_is_hr(file, currentLine)) {
        md_force_close_para();
        fprintf(md_outfile, "<hr />\n");
        break;
      }
      md_handle_list(file, currentLine);
      break;
    default:
      if (isspace(*file[currentLine]) != 0 || strlen(file[currentLine]) == 0)
        break;

      if (isdigit(file[currentLine][0])) {
        md_handle_numbered_list(file, currentLine);
      } else {
        if (currentLine + 1 == fileLength) {
          md_handle_para(file, currentLine);
          md_force_close_para();
          break;
        }

        md_is_single_para(file, currentLine) ? md_handle_single_para(file, currentLine)
                                             : md_handle_para(file, currentLine);
      }
    }
  }

  fclose(md_infile);
  fclose(md_outfile);
}
