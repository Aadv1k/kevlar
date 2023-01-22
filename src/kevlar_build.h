#ifndef KEVLAR_BUILD_H
#define KEVLAR_BUILD_H

#include "kevlar_handle_config.h"
#include "kevlar_new.h"
#include <stddef.h>

typedef struct ListingItem {
  char lTitle[CONFIG_MAX_PATH_SIZE];
  char lDate[CONFIG_MAX_PATH_SIZE];
  char lContent[CONFIG_MAX_FILE_SIZE];
  char lPath[CONFIG_MAX_PATH_SIZE];
  int lOrder;
} ListingItem;

#define BUILD_MAX_CMD_SIZE 256

void kevlar_check_if_theme_valid(const char theme_path[CONFIG_MAX_PATH_SIZE]);

void kevlar_check_if_kevlar_proj(const char folder_path[CONFIG_MAX_PATH_SIZE],
                                 KevlarSkeleton *skeleton);
void kevlar_handle_build_command(const char * folder_path);

size_t kevlar_count_files_in_folder(const char * folder_path, const char * filetype);

#endif
