#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "kevlar_build.h"
#include "kevlar_handle_config.h"
#include "kevlar_new.h"

// FOUND IN RST2HTML.c put it in a util
void truncateLast(char *target) {
  target[strlen(target)-1] = '\0';
}

void kevlar_check_if_kevlar_proj(char file_path[MAX_FOLDER_PATH_SIZE], KevlarSkeleton skeleton) {
  DIR * dir_stream; 
  struct dirent *dir_itm;

  // TODO: maybe this could be used here instead 
  // kevlar_get_folder_status(char *folder_path)?
  if ((dir_stream = opendir(file_path))== NULL) {
      fprintf(stderr, "[kevlar] ran into a problem while opening %s; it may not exist\n", file_path);
      exit(1);
  }

  const int skeleton_size = sizeof(skeleton)/MAX_FOLDER_PATH_SIZE;
  char items[skeleton_size][MAX_FOLDER_PATH_SIZE];

  int i = 0;
  while ((dir_itm = readdir(dir_stream)) != NULL && i < skeleton_size) {
    if (strcmp(dir_itm->d_name, ".") == 0 || strcmp(dir_itm->d_name, "..") == 0) {
      continue;
    }
    strcpy(items[i], dir_itm->d_name);
    i++;
  }

  truncateLast(skeleton.skel_posts_folder_path);
  truncateLast(skeleton.skel_template_folder_path);

  if (
    strcmp(items[0], skeleton.skel_template_folder_path) != 0 ||
    strcmp(items[1], skeleton.skel_posts_folder_path) != 0 ||
    strcmp(items[2], skeleton.skel_config_file_path) != 0) {
    fprintf(stderr, "[kevlar] %s doesn't seem to be a kevlar project, try running kevlar new for a new project\n", file_path);
    exit(1);
  }
}

void kevlar_handle_build_command(char file_path[MAX_FOLDER_PATH_SIZE], KevlarConfig *config) {
  KevlarSkeleton skel = {
    "templates/", 
    "posts/", 
    "config.ini"
  };

  kevlar_check_if_kevlar_proj(file_path, skel);

  DIR * dir_stream; 
  struct dirent *dir_itm;

  // We don't need handle this being NULL since kevlar_check_if_kevlar_proj()
  // already does so

  dir_stream = opendir(file_path);
}

/*
int main () {
  KevlarConfig config;
  kevlar_load_config("../../kevlar_proj/config.ini", &config);
  kevlar_handle_build_command("../../kevlar_proj", &config);
  return 0;
}*/
