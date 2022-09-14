#ifndef KEVLAR_HANDLE_CONFIG_H
#define KEVLAR_HANDLE_CONFIG_H

#define MAX_CONFIG_OPT_SIZE 32
#define MAX_CONFIG_FILE_SIZE 500
#define MAX_PATH_SIZE 64

typedef struct {
  char configAuthor[MAX_CONFIG_OPT_SIZE];
  char configTitle[MAX_CONFIG_OPT_SIZE];
  char configMarkdownLoader[MAX_CONFIG_OPT_SIZE];
  char configRstLoader[MAX_CONFIG_OPT_SIZE];
} KevlarConfig;

void kevlar_generate_skeleton_config(char file_path[MAX_PATH_SIZE]);
void kevlar_load_config(char file_path[MAX_PATH_SIZE], KevlarConfig* config_struct);

#endif
