#include "kevlar_md_to_html.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "kevlar_handle_config.h"
#include "kevlar_rst_to_html.h"

#define MD_HEADING_LEVEL 6

FILE *md_infile;
FILE *md_outfile;

void md_handleHash(char file[][RST_LINE_LENGTH], int line) {
    int hashCount = 0;

    while (1) {
        if (file[line][hashCount] != '#')
            break;
        hashCount++;
    }

    if (hashCount < MD_HEADING_LEVEL) {
        fprintf(md_outfile, "<h%d>%s</h%d>\n", hashCount,
                isspace(*(strrchr(file[line], '#') + 1))
                    ? strrchr(file[line], '#') + 2
                    : strrchr(file[line], '#') + 1,
                hashCount);
    } else {
        fprintf(md_outfile, "<h6>%s</h6>\n", strrchr(file[line], '#') + 1);
    }
}

void md_handleUnorderedList(char file[][RST_LINE_LENGTH], int line) {
    static bool ulOpen = false;
    if (file[line - 1][0] != file[line][0] && ulOpen == false) {
        fprintf(md_outfile, "<ul>\n\t<li>%s</li>\n",
                isspace(file[line][1]) ? &file[line][2] : &file[line][1]);
        ulOpen = true;
    } else if (ulOpen && file[line + 1][0] == file[line][0]) {
        fprintf(md_outfile, "\t<li>%s</li>\n",
                isspace(file[line][1]) ? &file[line][2] : &file[line][1]);
    } else if (ulOpen && file[line + 1][0] != file[line][0]) {
        fprintf(md_outfile, "\t<li>%s</li>\n</ul>\n",
                isspace(file[line][1]) ? &file[line][2] : &file[line][1]);
        ulOpen = false;
    }
}

void md_handleOrderedList(char file[][RST_LINE_LENGTH], int line) {
    static bool olOpen = false;
    if (file[line - 1][0] != file[line][0] && olOpen == false) {
        fprintf(md_outfile, "<ol>\n\t<li>%s</li>\n",
                isspace(file[line][1]) ? &file[line][2] : &file[line][1]);
        olOpen = true;
    } else if (olOpen && file[line + 1][0] == file[line][0]) {
        fprintf(md_outfile, "\t<li>%s</li>\n",
                isspace(file[line][1]) ? &file[line][2] : &file[line][1]);
    } else if (olOpen && file[line + 1][0] != file[line][0]) {
        fprintf(md_outfile, "\t<li>%s</li>\n</ol>\n",
                isspace(file[line][1]) ? &file[line][2] : &file[line][1]);
        olOpen = false;
    }
}

void md_handleSingleParagraph(char file[][RST_LINE_LENGTH], int line) {
    fprintf(md_outfile, "<p>%s</p>\n", file[line]);
}

void md_handleParagraph(char file[][RST_LINE_LENGTH], int line) {
    static bool paraOpen = false;

    if (paraOpen == false) {
        fprintf(md_outfile, "<p>%s", file[line]);
        paraOpen = true;
    } else if (paraOpen && strlen(file[line + 1]) > 1) {
        fprintf(md_outfile, " %s", file[line]);
    } else if (paraOpen && strlen(file[line + 1]) <= 2) {
        fprintf(md_outfile, " %s</p>\n", file[line]);
        paraOpen = false;
    }
}

bool md_isSingle(char file[][RST_LINE_LENGTH], int line) {
    // This is the simplest method to ensure/control what constitues a new line
    // para or a grouped para
    if (file[line + 1][0] == '-' || file[line + 1][0] == '*')
        return true;
    if ((isspace(*file[line + 1]) != 0 || strlen(file[line + 1]) == 0) &&
        (isspace(*file[line - 1]) != 0 || strlen(file[line - 1]) == 0))
        return true;

    return false;
}

void md_parse(char *in_file_path, char *out_file_path) {
    if (strcmp(strrchr(in_file_path, '.'), ".md") != 0) {
        fprintf(stderr,
                "[src/kevlar_md_to_html.c/md_parse()] %s doesn't seem to be a "
                "markdown file\n",
                in_file_path);
        exit(1);
    }

    md_infile = fopen(in_file_path, "r");
    md_outfile = fopen(out_file_path, "w");

    if (md_infile == NULL) {
        fprintf(stderr, "the file \"%s\" doesn't exist.\n", in_file_path);
        exit(1);
    }

    puts(in_file_path);

    long fileLength = rst_getFileLength(in_file_path);
    char file[fileLength][RST_LINE_LENGTH];

    for (int i = 0; i < fileLength; i++) {
        fgets(file[i], RST_LINE_LENGTH, md_infile);
        utl_truncateLast(file[i]);
    }

    for (long currentLine = 0; currentLine < fileLength; currentLine++) {
        switch (file[currentLine][0]) {
        case '#':
            md_handleHash(file, currentLine);
            break;
        case '*':
        case '-':
            md_handleUnorderedList(file, currentLine);
            break;
        default:
            if (isspace(*file[currentLine]) != 0 ||
                strlen(file[currentLine]) == 0)
                break;

            if (isdigit(file[currentLine][0])) {
                md_handleOrderedList(file, currentLine);
            } else {
                md_isSingle(file, currentLine)
                    ? md_handleSingleParagraph(file, currentLine)
                    : md_handleParagraph(file, currentLine);
            }
        }
    }

    fclose(md_infile);
    fclose(md_outfile);
}
