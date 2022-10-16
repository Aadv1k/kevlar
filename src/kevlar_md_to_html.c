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

static bool paraOpen = false;
static bool codeBlockOpen = false;
static bool ulOpen = false;
static bool olOpen = false;

bool md_is_code_block(const char * line) {
  if (utl_count_repeating_char(line[0], line) == 3) {
    return true;
  } 
  return false;
}

bool md_is_hr(const char * line) {
  if (utl_count_repeating_char(line[0], line) == 3) {
    return true;
  } 
  return false;
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

void md_force_close_para() {
  char tag[10] = "p";
  fprintf(md_outfile, "</%s>\n", tag);
  paraOpen = false;
}

void md_handle_code_block() {
  if (codeBlockOpen == false) {
    fprintf(md_outfile, "<pre>\n<code>\n");
    codeBlockOpen = true;
  } else if (codeBlockOpen) {
    fprintf(md_outfile, "</code>\n</pre>\n");
    codeBlockOpen = false;
  }
}

void md_handleText(char input[RST_LINE_LENGTH], char output[RST_LINE_LENGTH]) {
  bool boldOpen = false;
  bool emOpen = false;
  bool tickOpen = false;
  bool tildeOpen = false;
  bool underScoreOpen = false;

  for (int i = 0; input[i] != '\0'; i++) {
    if (input[i] == '*') {
      switch (utl_count_repeating_char('*', &input[i])) {
      case 1:
        !emOpen ? strcat(output, "<em>") : strcat(output, "</em>");
        emOpen = !emOpen;
        break;
      case 2:
        !boldOpen ? strcat(output, "<b>") : strcat(output, "</b>");
        boldOpen = !boldOpen;
        break;
      }
      i += utl_count_repeating_char('*', &input[i]) - 1;
    } else if (input[i] == '`') {
      !tickOpen ? strcat(output, "<code>") : strcat(output, "</code>");
      i += utl_count_repeating_char('`', &input[i]) - 1;
      tickOpen = !tickOpen;
    } else if (input[i] == '~'  && !tickOpen)  {
      !tildeOpen ? strcat(output, "<del>") : strcat(output, "</del>");
      i += utl_count_repeating_char('~', &input[i]) - 1;
      tildeOpen = !tildeOpen;
    }  else if (input[i] == '_'  && !tickOpen) {
      !underScoreOpen ? strcat(output, "<em>") : strcat(output, "</em>");
      i += utl_count_repeating_char('_', &input[i]) - 1;
      underScoreOpen = !underScoreOpen;
    } else if (input[i] == '['  && !tickOpen) {

      char linkText[MD_LINK_SIZE] = ""; 
      char linkLink[MD_LINK_SIZE] = "";

      if (strchr(&input[i], ']') == NULL) {
        kevlar_warn("[%s] malformed link!", __FILE__);
        strncat(output, &input[i], 1);
        continue;
      }
      
      // skip first since it would be '['
      int textChrCount = 1;
      while (&input[i+textChrCount] != strchr(&input[i], ']')) {
        strncat(linkText, &input[i+textChrCount], 1);
        textChrCount++;
      }

      if (strchr(&input[i], ')') == NULL || strchr(&input[i], '(') == NULL) {
        kevlar_warn("[%s] malformed link!", __FILE__);
        strncat(output, &input[i], 1);
        continue;
      }

      
      // skip this since it will be '(' 
      int linkChrCount = 1 + textChrCount;
      while (&input[i+linkChrCount] != strchr(&input[i], ')')) {
        strncat(linkLink, &input[i+linkChrCount], 1);
        linkChrCount++;
      }

      char linkStr[RST_LINE_LENGTH];
      
      if (input[i-1] == '!') {
        snprintf(linkStr, RST_LINE_LENGTH, "<img src=\"%s\">%s</img>", linkLink+1, linkText);
        utl_truncateLast(output);
      } else {
        snprintf(linkStr, RST_LINE_LENGTH, "<a href=\"%s\">%s</a>", linkLink+1, linkText);
      }

      strcat(output, linkStr);
      i += linkChrCount;
    } else {
      strncat(output, &input[i], 1);
    }
  }

  if (boldOpen || emOpen) {
    kevlar_warn("[md2html] some asterisks were never closed!");
  }

}

void md_handle_heading(char file[][RST_LINE_LENGTH], int line) {
  size_t hashCount = utl_count_repeating_char('#', file[line]);

  if (hashCount < MD_HEADING_LEVEL) {
    fprintf(md_outfile, "<h%zu>%s</h%zu>\n", hashCount,
            isspace(*(strrchr(file[line], '#') + 1)) ? strrchr(file[line], '#') + 2
                                                     : strrchr(file[line], '#') + 1,
            hashCount);
  } else {
    fprintf(md_outfile, "<h6>%s</h6>\n", strrchr(file[line], '#') + 1);
  }
}


void md_handle_list(char file[][RST_LINE_LENGTH], int line) {
  char *target_line = isspace(file[line][2]) ? &file[line][3] : &file[line][2];
  char target[RST_LINE_LENGTH] = "";
  md_handleText(target_line, target);

  if (!ulOpen && file[line - 1][0] != file[line][0]) {
    fprintf(md_outfile, "\n<ul>\n\t<li>%s</li>\n", target);
    ulOpen = true;

    if (file[line + 1][0] != file[line][0]) {
      fprintf(md_outfile, "</ul>\n");
      ulOpen = false;
    }

  } else if (ulOpen && file[line + 1][0] == file[line][0]) {
    fprintf(md_outfile, "\t<li>%s</li>\n", target);
  } else if (ulOpen && file[line + 1][0] != file[line][0]) {
    fprintf(md_outfile, "\t<li>%s</li>\n</ul>\n", target);
    ulOpen = false;
  }
}


void md_handle_numbered_list(char file[][RST_LINE_LENGTH], int line) {
  char *target_line = isspace(file[line][2]) ? &file[line][3] : &file[line][2];
  char target[RST_LINE_LENGTH] = "";
  md_handleText(target_line, target);

  if (!olOpen && file[line - 1][0] != file[line][0]) {
    fprintf(md_outfile, "\n<ol>\n\t<li>%s</li>\n", target);
    olOpen = true;

    if (!isdigit(file[line + 1][0]) && file[line+1][1] != '.') {
      fprintf(md_outfile, "</ol>\n");
      olOpen = false;
    }

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


void md_handle_para(char file[][RST_LINE_LENGTH], int line) {
  char parsed_output[RST_LINE_LENGTH] = "";
  md_handleText(file[line], parsed_output);

  char tag[10] = "p";

  if (!paraOpen) {
    fprintf(md_outfile, "<%s>%s", tag, parsed_output);
    paraOpen = true;
  } else if (paraOpen && strlen(file[line + 1]) > 1) {
    fprintf(md_outfile, " %s", parsed_output);
  } else if (paraOpen && strlen(file[line + 1]) <= 2) {
    fprintf(md_outfile, " %s</%s>\n", parsed_output, tag);
    paraOpen = false;
  }
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

  printf("Parsing %s\n", in_file_path);
  
  for (int i = 0; i < fileLength; i++) {
    if (!fgets(file[i], RST_LINE_LENGTH, md_infile))
      kevlar_warn("something went wrong");
    utl_truncateLast(file[i]);
  }

  for (long currentLine = 0; currentLine < fileLength; currentLine++) {
    switch (file[currentLine][0]) {
    case '#':
    if (codeBlockOpen) {
      fprintf(md_outfile, "%s\n", file[currentLine]);
      break;
    }
      md_handle_heading(file, currentLine);
      break;
    case '`': 
      if (md_is_code_block(file[currentLine])) {
        md_handle_code_block();
      }
      break;
    case '*':
    case '-':

    if (codeBlockOpen) {
      fprintf(md_outfile, "%s\n", file[currentLine]);
      break;
    }
      if (md_is_hr(file[currentLine])) {
        md_force_close_para();
        fprintf(md_outfile, "<hr />\n");
        break;
      }
      if (file[currentLine][1] == ' ') {
        md_handle_list(file, currentLine);
        break;
      }
      // fall-through
    default:

      if (isspace(*file[currentLine]) != 0 || strlen(file[currentLine]) == 0)
        break;

      if (codeBlockOpen) {
        fprintf(md_outfile, "%s\n", file[currentLine]);
        break;
      }
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
