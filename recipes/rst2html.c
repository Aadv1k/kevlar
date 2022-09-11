#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rst2html.h"

bool ERRORS = false;

int rst_getFileLen(char filename[]) {
  FILE *infile = fopen(filename, "r");
  int i;
  while (!feof(infile)) {
    if (getc(infile) == '\n') {
      i++;
    }
  }
  return i;
}

int rst_isTextOnly(char file[][LINELEN], int line) {
  // TODO: this "just works" and the more tokens we add, the more complex our
  // setup will get, so fix this to a better implementation Maybe an enum with
  // all the tokens?
  if ((file[line + 1][0] != '-' && file[line - 1][0] != '-') &&
      (file[line + 1][0] != '=' && file[line - 1][0] != '=') &&
      (file[line + 1][0] != ' ' && file[line - 1][0] != ' ')) {
    return 1;
  }
  return 0;
}

void rst_handleEqual(char file[][LINELEN], int line) {
  static int equalOpen = 0;
  if (line == equalOpen && line != 0)
    return;

  if (strcmp(file[line + 2], file[line]) == 0 &&
      strlen(file[line]) == strlen(file[line + 2])) {

    equalOpen = line + 2;

    int len = strlen(file[line + 1]);
    char chopped[len];
    strcpy(chopped, file[line + 1]);
    chopped[len - 1] = '\0';
    fprintf(outfile, "<h1>%s</h1>\n", chopped);

  } else if (strlen(file[line - 1]) == strlen(file[line]) &&
             strcmp(file[line - 2], file[line])) {
    int len = strlen(file[line - 1]);
    char chopped[len];
    strcpy(chopped, file[line - 1]);
    chopped[len - 1] = '\0';
    fprintf(outfile, "\n<h2>%s</h2>\n", chopped);
  } else {
    if (ERRORS) {
      printf("[rst2html] Error while parsing title\n %d | %s\n>%d | %s %d | %s",
             line, file[line - 1], line + 1, file[line], line + 2,
             file[line + 1]);
      exit(1);
    }
  }
}

void rst_handleText(char file[][LINELEN], int line, bool newLine) {
  bool open = true;
  int style = 0;
  char res[999] = "";
  char content[strlen(file[line])];
  strcpy(content, file[line]);
  // Brace yourselves for whats about to come, it is NOT pretty

  for (int i = 0; content[i] != '\n'; i++) {
    // If we find an asterisk, we add to the style, style can keep track of how
    // many astersiks we have
    if (content[i] == '*') {
      style++;

      // If there is some content after the asterisk and we put an opening tag
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
    } else if (content[i] != '*') {
      // We reset the style to 0 otherwise it will keep on adding, ideally, we
      // can extend this setup infinitely given how we can keep track of how
      // many asterisks we have
      strncat(res, &content[i], 1);
      style = 0;
    }
  }

  char *chopped;

  if (res[0] == '-' && res[1] == ' ') {
    chopped = res + 2;
  } else {
    chopped = res;
  }

  if (newLine) {
    fprintf(outfile, "\n<p>%s</p>\n", chopped);
  } else {
    fprintf(outfile, "<p>%s</p>", chopped);
  }
}

void rst_handleDashAndUl(char file[][LINELEN], int line) {
  static bool listOpen = false;

  if (strlen(file[line]) == strlen(file[line - 1]) && listOpen != 1) {
    // HANDLE H3
    int len = strlen(file[line - 1]);
    char chopped[len];
    strcpy(chopped, file[line - 1]);
    chopped[len - 1] = '\0';
    fprintf(outfile, "\n<h3>%s</h3>\n", chopped);

  } else if (file[line][1] == ' ' && listOpen == 0 &&
             strlen(file[line - 1]) == 1) {
    listOpen = true;

    fprintf(outfile, "<ul>\n\t<li>\n\t\t");
    rst_handleText(file, line, 0);
    fprintf(outfile, "\n\t</li>\n");

  } else if (file[line][1] == ' ' && listOpen == 1 &&
             strlen(file[line + 1]) != 1) {
    // ADD TO THE LIST;
    // rst_handleText(file, line, 0);
    fprintf(outfile, "\t<li>\n\t");
    rst_handleText(file, line, 0);
    fprintf(outfile, "</li>\n");

  } else if (file[line][1] == ' ' && listOpen == 1 &&
             strlen(file[line + 1]) == 1) {

    fprintf(outfile, "\t<li>\n\t\t");
    rst_handleText(file, line, 0);
    fprintf(outfile, "\n\t</li>\n</ul>\n\n");

    // CLOSE THE LIST

    listOpen = false;
  } else {
    if (ERRORS) {
      printf("%s\n[rst2html] LINE-%d: Error while parsing list item or dashed "
             "title\n",
             file[line], line + 1);
      exit(1);
    }
  }
}

void rst_handleConfig(char file[][LINELEN], int line) {
  char param[PARAMSIZE] = "";
  char opt[PARAMSIZE] = "";
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
  puts("rst2html -[OPT] INPUT.rst OUTPUT.html\n\t-h -- On invalid rst exit "
       "with error message\n");
  exit(2);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    usage();
  }

  char arg1[50], arg2[50];

  int opt;
  while ((opt = getopt(argc, argv, ":h:")) != -1) {
    switch (opt) {
    case 'h':
      ERRORS = true;
      strcpy(arg1, optarg);
      strcpy(arg2, argv[optind]);
      break;
    case ':':
    case '?':
      usage();
      break;
    }
  }

  // It works ¯\_(ツ)_/¯
  if (!ERRORS) {
    strcpy(arg1, argv[1]);
    strcpy(arg2, argv[2]);
  }

  infile = fopen(arg1, "r");
  outfile = fopen(arg2, "w");

  if (infile == NULL) {
    printf("the file \"%s\" doesn't exist.\n", arg1);
    exit(1);
  }

  int fileLen = rst_getFileLen(arg1);
  char arr[fileLen][LINELEN];

  for (int i = 0; i < fileLen; i++) {
    fgets(arr[i], LINELEN, infile);
  }

  for (int i = 0; i < fileLen; i++) {
    switch (arr[i][0]) {
    case '=':
      rst_handleEqual(arr, i);
      break;
    case '-':
      rst_handleDashAndUl(arr, i);
      break;
    case ':':
      rst_handleConfig(arr, i);
      break;
    case '\n':
    case '\r':
    case ' ':
      // TODO: rst_handleSpace(arr, i);
      break;
    default:
      if (rst_isTextOnly(arr, i) == 1) {
        rst_handleText(arr, i, 1);
      }
    }
  }

  fclose(infile);
  fclose(outfile);
  return 0;
}
