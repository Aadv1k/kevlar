#ifndef KEVLAR_NEW_H
#define KEVLAR_NEW_H

#define NEW_SYS_CMD_LEN 256
#define FOLDER_ALL_PERMS 0777

#include "kevlar_handle_config.h"

typedef struct KevlarSkeleton {
  char skel_template_folder_path[CONFIG_MAX_PATH_SIZE];
  char skel_posts_folder_path[CONFIG_MAX_PATH_SIZE];
  char skel_config_file_path[CONFIG_MAX_PATH_SIZE];
  char skel_dist_file_path[CONFIG_MAX_PATH_SIZE];
} KevlarSkeleton;

enum FolderStatus { folderNull = 2, folderEmpty, folderNonEmpty };

int kevlar_get_folder_status(const char * folder_path);

void kevlar_generate_new_skeleton(KevlarSkeleton *skeleton);

void kevlar_handle_new_command(char folder_path[CONFIG_MAX_PATH_SIZE]);

void kevlar_generate_new_post(const char * folder_path, char * post_name);

#endif
