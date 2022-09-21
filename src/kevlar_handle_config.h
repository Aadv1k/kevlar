#ifndef KEVLAR_HANDLE_CONFIG_H
#define KEVLAR_HANDLE_CONFIG_H

#define CONFIG_MAX_OPT_SIZE 32
#define CONFIG_MAX_FILE_SIZE 100000
#define CONFIG_MAX_PATH_SIZE 256

typedef struct {
  char configAuthor[CONFIG_MAX_OPT_SIZE];
  char configTitle[CONFIG_MAX_OPT_SIZE];
  char configMarkdownLoader[CONFIG_MAX_OPT_SIZE];
  char configRstLoader[CONFIG_MAX_OPT_SIZE];
  char configTheme[CONFIG_MAX_OPT_SIZE];
  char configListing[CONFIG_MAX_FILE_SIZE];

  char configHeaderPath[CONFIG_MAX_OPT_SIZE];
  char configFooterPath[CONFIG_MAX_OPT_SIZE];
  char configHtmlContents[CONFIG_MAX_FILE_SIZE];
  char configIndexPath[CONFIG_MAX_OPT_SIZE];
  char configPostPath[CONFIG_MAX_OPT_SIZE];

} KevlarConfig;

void kevlar_generate_skeleton_config(char file_path[CONFIG_MAX_PATH_SIZE]);

void kevlar_load_config(char file_path[CONFIG_MAX_PATH_SIZE], KevlarConfig* config_struct);

#endif
