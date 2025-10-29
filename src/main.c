#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kevlar_build.h"
#include "kevlar_errors.h"
#include "kevlar_new.h"

#define MAX_CMD_SIZE 16

enum COMMANDS { cmdHelp, cmdNew, cmdBuild, cmdNewPost, cmdTail };

int argtype(char arg[]) {
    char param[][MAX_CMD_SIZE] = {"help", "new", "build", "new-post"};

    assert(cmdTail == sizeof(param) / MAX_CMD_SIZE);

    for (int i = 0; i < cmdTail; i++) {
        if (!strcmp(param[i], arg))
            return i;
    }
    return -1;
}

void kevlar_usage_exit(const char *origin) {
    printf("%s <cmd> <opt>\n", origin);
    printf("  help       -- print this help message\n");
    printf("  new        -- create a new site skeleton\n");
    printf("  new-post   -- create a new post with defaults\n");
    printf("  build      -- build if in a kevlar project\n");
    exit(0);
}

int main(int argc, char **argv) {
    if (argc == 1) {
        kevlar_usage_exit(argv[0]);
    }

    switch (argtype(argv[1])) {
    case cmdHelp:
        kevlar_usage_exit(argv[0]);
        break;
    case cmdNew:
        if (argc == 2) {
            kevlar_err("you need to provide a name for your project!");
        }
        kevlar_handle_new_command(argv[2]);
        break;
    case cmdBuild:
        kevlar_handle_build_command(".");
        break;
    case cmdNewPost:
        if (argc == 2) {
            kevlar_err("you need to provide a title for your post!");
        }
        kevlar_generate_new_post(".", argv[2]);
        break;
    default:
        kevlar_err("couldn't find command \"%s\"", argv[1]);
    }

    return 0;
}
