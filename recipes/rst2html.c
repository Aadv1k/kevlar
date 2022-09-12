#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 999
#define FIELD_LIST_SIZE 50

FILE *infile; 
FILE *outfile;

bool ERRORS = false;

void truncateLast(char *target) {
  target[strlen(target)-1] = '\0';
}

int rst_getFileLength(char filename[]) {
  FILE *infile = fopen(filename, "r");
  int i;
  while (!feof(infile)) {
    if (getc(infile) == '\n') {
      i++;
    }
  }
  return i;
}

int rst_isTextOnly(char file[][MAX_LINE_LENGTH], int line) {
  if (strlen(file[line]) == 0) return 0;
  // TODO: This is pretty messy and might cause troubles later on
  if ((file[line + 1][0] != '-' && file[line - 1][0] != '-') &&
      (file[line + 1][0] != '=' && file[line - 1][0] != '=') &&
      (file[line + 1][0] != ' ' && file[line - 1][0] != ' ')) {
    return 1;
  }
  return 0;
}

void rst_handleEqual(char file[][MAX_LINE_LENGTH], int line) {
  static int equalOpen = 0;

  if (line == equalOpen && line != 0) return;

  if (strcmp(file[line + 2], file[line]) == 0 &&
      strlen(file[line]) == strlen(file[line + 2])) {

    equalOpen = line + 2;

    fprintf(outfile, "<h1>%s</h1>\n", file[line+1]);

  } else if (strlen(file[line - 1]) == strlen(file[line]) && 
    strcmp(file[line - 2], file[line])) {
    fprintf(outfile, "\n<h2>%s</h2>\n", file[line-1]);

  } else {
    if (ERRORS) {
      printf("[rst2html] Error while parsing title\n %d | %s\n>%d | %s %d | %s",
             line, file[line - 1], line + 1, file[line], line + 2,
             file[line + 1]);
      exit(1);
    }
  }
}

void rst_handleText(char file[][MAX_LINE_LENGTH], int line, bool newLine) {
  bool open = true;
  int style = 0;
  char res[999] = "";
  char content[strlen(file[line])];
  strcpy(content, file[line]);
  // Brace yourselves for whats about to come, it is NOT pretty

  for (int i = 0; content[i] != '\0'; i++) {
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

void rst_handleDashAndUl(char file[][MAX_LINE_LENGTH], int line) {
  static bool listOpen = false;

  if (strlen(file[line]) == strlen(file[line - 1]) && listOpen != 1) {
    
    // It a header with dashed underline or H3

    fprintf(outfile, "\n<h3>%s</h3>\n", file[line-1]);
  } else if (file[line][1] == ' ' && listOpen == 0 &&
             strlen(file[line - 1]) == 0) {

    // Start of the list, we append a <ul> at the beginning

    listOpen = true;

    fprintf(outfile, "<ul>\n\t<li>\n\t\t");
    rst_handleText(file, line, 0);
    fprintf(outfile, "\n\t</li>\n");

  } else if (file[line][1] == ' ' && listOpen == 1 &&
             strlen(file[line + 1]) != 0) {
    
    // This is the middle of the list, we don't need to surround by a <ul> or </ul>
    

    fprintf(outfile, "\t<li>\n\t");
    rst_handleText(file, line, 0);
    fprintf(outfile, "</li>\n");

  } else if (file[line][1] == ' ' && listOpen == 1 &&
             strlen(file[line + 1]) == 0) {

    // End of the list, we append a </ul> at the end to close the list

    listOpen = false;

    fprintf(outfile, "\t<li>\n\t\t");
    rst_handleText(file, line, 0);
    fprintf(outfile, "\n\t</li>\n</ul>\n\n");
  } else {
    if (ERRORS) {
      printf("%s\n[rst2html] LINE-%d: Error while parsing list item or dashed "
             "title\n",
             file[line], line + 1);
      exit(1);
    }
  }
}

void rst_handleConfig(char file[][MAX_LINE_LENGTH], int line) {
  char param[FIELD_LIST_SIZE] = "";
  char opt[FIELD_LIST_SIZE] = "";
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

  char rst_file_path[50], html_file_path[50];

  int opt;
  while ((opt = getopt(argc, argv, ":h:")) != -1) {
    switch (opt) {
    case 'h':
      ERRORS = true;
      strcpy(rst_file_path, optarg);
      strcpy(html_file_path, argv[optind]);
      break;
    case ':':
    case '?':
      usage();
      break;
    }
  }

  if (!ERRORS) {
    strcpy(rst_file_path, argv[1]);
    strcpy(html_file_path, argv[2]);
  }

  infile = fopen(rst_file_path, "r");
  outfile = fopen(html_file_path, "w");

  if (infile == NULL) {
    printf("the file \"%s\" doesn't exist.\n", rst_file_path);
    exit(1);
  }

  int fileLength = rst_getFileLength(rst_file_path);
  char file[fileLength][MAX_LINE_LENGTH];

  // Read contents of the input file into the file[]
  for (int i = 0; i < fileLength; i++) {
    fgets(file[i], MAX_LINE_LENGTH, infile);
    truncateLast(file[i]);
  }
  
  for (int currentLine = 0; currentLine < fileLength; currentLine++) {
    switch (file[currentLine][0]) {
    case '=':
      rst_handleEqual(file, currentLine);
      break;
    case '-':
      rst_handleDashAndUl(file, currentLine);
      break;
    case ':':
      rst_handleConfig(file, currentLine);
      break;
    case '\n':
    case '\r':
    case ' ':
      // TODO: rst_handleSpace(arr, i);
      break;
    default:
      if (rst_isTextOnly(file, currentLine) == 1) {
        // TODO: Clean code principal - create a different implementation of this functiion that handles the new line 
        rst_handleText(file, currentLine, 1);
      }
    }
  }

  fclose(infile);
  fclose(outfile);
  return 0;
}

