#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "kevlar_errors.h"
#include "kevlar_handle_config.h"
#include "kevlar_new.h"

void trimWhitespace(char *str) {
    int len = strlen(str);
    while (len > 0 && isspace(str[len - 1])) {
        str[len - 1] = '\0';
        len--;
    }
}

void kevlar_get_opt_from_config(char *file_path, char *opt, char *arg) {
    FILE *file_buf = fopen(file_path, "r");
    if (file_buf == NULL) {
        fprintf(stderr, "[kevlar] Error opening file: %s\n", file_path);
        exit(1);
    }

    // Convert the option to lowercase for case-insensitive matching
    char curOpt[CONFIG_MAX_OPT_SIZE];
    strncpy(curOpt, opt, CONFIG_MAX_OPT_SIZE - 1);
    curOpt[CONFIG_MAX_OPT_SIZE - 1] = '\0';
    for (int i = 0; curOpt[i] != '\0'; i++) {
        curOpt[i] = tolower(curOpt[i]);
    }

    char cur_line[CONFIG_MAX_FILE_SIZE];

    while (fgets(cur_line, CONFIG_MAX_FILE_SIZE, file_buf) != NULL) {
        // Ignore lines starting with '#'
        if (cur_line[0] == '#') {
            continue;
        }

        trimWhitespace(cur_line);

        char command[2][CONFIG_MAX_OPT_SIZE] = { "", "" };

        char *token = strtok(cur_line, "=");
        int i = 0;
        while (token != NULL && i < 2) {
            strncpy(command[i], token, CONFIG_MAX_OPT_SIZE - 1);
            command[i][CONFIG_MAX_OPT_SIZE - 1] = '\0';
            token = strtok(NULL, "=");
            trimWhitespace(command[i]);
            i++;
        }

        if (strlen(command[0]) == 0 || strlen(command[1]) == 0) {
            continue;
        }

        // Convert the option name from the config file to lowercase
        for (int i = 0; command[0][i] != '\0'; i++) {
            command[0][i] = tolower(command[0][i]);
        }

        if (strcmp(command[0], curOpt) == 0) {
            strncpy(arg, command[1], CONFIG_MAX_OPT_SIZE - 1);
            arg[CONFIG_MAX_OPT_SIZE - 1] = '\0';
            fclose(file_buf);
            return;
        }
    }

    // Option not found in the config file
    fprintf(stderr, "[kevlar] Option \"%s\" not found in %s\n", opt, file_path);
    fclose(file_buf);
    exit(1);
}

void kevlar_generate_skeleton_config(char file_path[CONFIG_MAX_PATH_SIZE]) {
  FILE *file_buf = fopen(file_path, "w");
  if (!file_buf)
    kevlar_err("something went wrong while generating sample config\n");
  // This works for now since the file we will be generating won't be that big
  fprintf(file_buf, "title=Begin\ntheme=listed-kevlar-theme\n");
  fclose(file_buf);
}
