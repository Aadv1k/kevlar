#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "kevlar_new.h"

#define MAX_CMD_SIZE 16

enum COMMANDS {
  help,
  new,
  serve,
  server,
  build,
  end
};

int argtype(char arg[]) {
  enum COMMANDS com;
  char param[][MAX_CMD_SIZE] = {
    "help",
    "new",
    "serve",
    "server",
    "build"
  };
  for (int i = 0; i < end; i++) {
    assert(end == sizeof(param)/MAX_CMD_SIZE);
    if (!strcmp(param[i], arg)) return i;
  }
  return -1;
}

void kevlar_usage_exit() {
  printf("kevlar <COMMAND> <OPT>\n\thelp -- print this help message\n\tnew -- create a new site skeleton\n");
  exit(2);
}

int main(int argc, char **argv) {
  if (argc == 1) { 
    kevlar_usage_exit();
  }

  switch(argtype(argv[1])) {
    case 0:
      kevlar_usage_exit();
      break;
    case 1:
      if (argc == 2) {
        fprintf(stderr, "[kevlar] you need an provide a name for your project\n");
        exit(2);
      }
      kev_handle_new_command(argv[3]);
      break;
    case 2: 
    case 3: 
      puts("SERVE CONTENT");
      break;
    default: 
      puts("COMMAND NOT FOUND");
  }

  return 0;
}
