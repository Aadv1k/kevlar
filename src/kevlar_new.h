#ifndef KEVLAR_NEW_H
#define KEVLAR_NEW_H

#define MAX_FOLDER_PATH_SIZE 200
#define FOLDER_ALL_PERMS 0777

typedef struct KevlarSkeleton {
  char skel_template_folder_path[MAX_FOLDER_PATH_SIZE];
  char skel_posts_folder_path[MAX_FOLDER_PATH_SIZE];
  char skel_config_file_path[MAX_FOLDER_PATH_SIZE];
} KevlarSkeleton;

enum FolderStatus {
  folderNull,
  folderEmpty,
  folderNonEmpty
};

int kev_folder_status(char folder_path[MAX_FOLDER_PATH_SIZE]);

void kev_new_skeleton(KevlarSkeleton *skeleton);

void kev_handle_new_command(char folder_path[MAX_FOLDER_PATH_SIZE]);

#endif
