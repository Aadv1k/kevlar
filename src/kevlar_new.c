#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "kevlar_new.h"
#include "kevlar_handle_config.h"

// TODO: Put this in util
void prepend(char prefix[], char str[]) {
  char *temp_str = strdup(str);
  strcpy(str, prefix);
  strcat(str, temp_str);
  free(temp_str);
}

int kevlar_get_folder_status(char folder_path[MAX_FOLDER_PATH_SIZE]) {
  DIR *dir_stream;
  struct dirent *dir_obj; 

  dir_stream = opendir(folder_path);

  if (dir_stream == NULL) return folderNull;

  dir_obj = readdir(dir_stream);
  if (strcmp(dir_obj->d_name, ".") != 0 && 
    strcmp(dir_obj->d_name, "..") != 0) return folderNonEmpty;

  return folderEmpty;
}

void kevlar_generate_new_skeleton(KevlarSkeleton *skeleton) {
  if (mkdir(skeleton->skel_posts_folder_path, FOLDER_ALL_PERMS) == -1 || 
    mkdir(skeleton->skel_template_folder_path, FOLDER_ALL_PERMS) == -1)
  {
    fprintf(stderr, "[kevlar] Something went wrong while creating skeleton\n");
    exit(-1);
  }

  kevlar_generate_skeleton_config(skeleton->skel_config_file_path);
  printf("[kevlar] Successfully created the skeleton; happy hacking ✨!\n");
}

void kevlar_handle_new_command(char folder_path[MAX_FOLDER_PATH_SIZE]) {
  switch (kevlar_get_folder_status(folder_path)) {
    case folderNonEmpty:
      fprintf(stderr, "[kevlar] folder \"%s\" already exists and is not empty!\n", folder_path);
      exit(1);
      break;
    case folderNull: 
      printf("[kevlar] \"%s\" not found, creating new folder\n", folder_path);
      mkdir(folder_path, 0777);
      [[__fallthrough__]];

    case folderEmpty: ;

      KevlarSkeleton skel = {
        "/templates/", 
        "/posts/", 
        "/config.ini"
      };

      prepend(folder_path, skel.skel_template_folder_path);
      prepend(folder_path, skel.skel_posts_folder_path);
      prepend(folder_path, skel.skel_config_file_path);

      kevlar_generate_new_skeleton(&skel);
      break; 
  }
}
