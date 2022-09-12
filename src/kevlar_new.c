#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


#include "kevlar_new.h"

// TODO: Put this in util
void prepend(char prefix[], char str[]) {
  char *temp_str = strdup(str);
  strcpy(str, prefix);
  strcat(str, temp_str);
  free(temp_str);
}

int kev_folder_status(char folder_path[MAX_FOLDER_PATH_SIZE]) {
  DIR *dir_stream;
  struct dirent *dir_obj; 

  dir_stream = opendir(folder_path);

  if (dir_stream == NULL) return folderNull;

  dir_obj = readdir(dir_stream);
  if (strcmp(dir_obj->d_name, ".") != 0 && 
    strcmp(dir_obj->d_name, "..") != 0) return folderNonEmpty;

  return folderEmpty;
}

void kev_new_skeleton(KevlarSkeleton *skeleton) {
  FILE *config_file;

  if (mkdir(skeleton->skel_posts_folder_path, FOLDER_ALL_PERMS) == -1 || 
    mkdir(skeleton->skel_template_folder_path, FOLDER_ALL_PERMS) == -1 || 
    (config_file = fopen(skeleton->skel_config_file_path, "w")) == NULL)
  {
    fprintf(stderr, "[kevlar] Something went wrong while creating skeleton\n");
    fclose(config_file);
    exit(-1);
  }

  printf("[kevlar] Successfully created the skeleton; happy hacking ✨!\n");
  fclose(config_file);
}

void kev_handle_new_command(char folder_path[MAX_FOLDER_PATH_SIZE]) {
  switch (kev_folder_status(folder_path)) {
    case folderNonEmpty:
      fprintf(stderr, "[kevlar] folder \"%s\" already exists and is not empty!\n", folder_path);
      exit(1);
      break;
    case folderNull: 
      printf("[kevlar] \"%s\" not found, creating new folder\n", folder_path);
      mkdir(folder_path, 0777);
    case folderEmpty: ;

      KevlarSkeleton skel = {
        "/templates/", 
        "/posts/", 
        "/config.toml"
      };

      prepend(folder_path, skel.skel_template_folder_path);
      prepend(folder_path, skel.skel_posts_folder_path);
      prepend(folder_path, skel.skel_config_file_path);

      kev_new_skeleton(&skel);
      break; 
  }
}
