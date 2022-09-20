#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "kevlar_handle_config.h"
#include "kevlar_new.h"
#include "kevlar_templating.h"
#include "kevlar_build.h"

#define MAX_CMD_SIZE 16

enum COMMANDS {
  cmdHelp,
  cmdNew,
  cmdBuild,
  cmdServe,
  cmdServer,
  cmdTail
};

int argtype(char arg[]) {
  char param[][MAX_CMD_SIZE] = {
    "help",
    "new",
    "build",
    "serve",
    "server",
  };
  for (int i = 0; i < cmdTail; i++) {
    assert(cmdTail == sizeof(param)/MAX_CMD_SIZE);
    if (!strcmp(param[i], arg)) return i;
  }
  return -1;
}

void kevlar_usage_exit() {
  printf("kevlar <COMMAND> <OPT>\n\thelp -- print this help message\n\tnew -- create a new site skeleton\n\tbuild -- build if in a kevlar project\n");
  exit(1);
}

int main(int argc, char **argv) {
  if (argc == 1) { 
    kevlar_usage_exit();
  }

  switch(argtype(argv[1])) {
    case cmdHelp:
      kevlar_usage_exit();
      break;
    case cmdNew:
      if (argc == 2) {
        fprintf(stderr, "[kevlar] you need an provide a name for your project\n");
        exit(1);
      }
      kevlar_handle_new_command(argv[2]);
     break;
    case cmdBuild: ;
      kevlar_handle_build_command(".");
      break;
    case cmdServe: 
    case cmdServer: 
      puts("SERVE CONTENT");
      break;
    default:
      fprintf(stderr, "[kevlar] couldn't find command\n");
      exit(127);
  }

  return 0;
}
