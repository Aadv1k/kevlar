#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


#if __has_include("windows.h")

#include <windows.h>

#endif

#include "kevlar_new.h"
#include "kevlar_handle_config.h"
#include "../utils/utils.h"

// TODO: Use stat here
int kevlar_get_folder_status(const char folder_path[CONFIG_MAX_PATH_SIZE]) {
  DIR *dir_stream;
  struct dirent *dir_obj; 

  dir_stream = opendir(folder_path);

  if (dir_stream == NULL) return folderNull;

  dir_obj = readdir(dir_stream);
  if (strcmp(dir_obj->d_name, ".") != 0 ||
    strcmp(dir_obj->d_name, "..") != 0) return folderNonEmpty;

  return folderEmpty;
}

void kevlar_generate_new_skeleton(KevlarSkeleton *skeleton) {
   #if defined(_WIN32)
    if (CreateDirectory(skeleton->skel_posts_folder_path, FOLDER_ALL_PERMS) == -1 || 
      CreateDirectory(skeleton->skel_template_folder_path, FOLDER_ALL_PERMS) == -1)
    {
      fprintf(stderr, "[kevlar] Something went wrong while creating skeleton\n");
      exit(1);
    }
   #else 
    if (mkdir(skeleton->skel_posts_folder_path, FOLDER_ALL_PERMS) == -1 || 
      mkdir(skeleton->skel_template_folder_path, FOLDER_ALL_PERMS) == -1)
    {
      fprintf(stderr, "[kevlar] Something went wrong while creating skeleton\n");
      exit(1);
    }
   #endif

  kevlar_generate_skeleton_config(skeleton->skel_config_file_path);

  char clone_git_command[NEW_SYS_CMD_LEN];  
  snprintf(clone_git_command, NEW_SYS_CMD_LEN, "git clone https://github.com/aadv1k/kyudo %s/kyudo", skeleton->skel_template_folder_path);
  system(clone_git_command);

  // TODO: THIS IS TEMPORARYa
  char write_post_command[NEW_SYS_CMD_LEN];
  snprintf(write_post_command, NEW_SYS_CMD_LEN, "echo 'Welcome to kevlar!\n==================\n\nif you are seeing this, everything has worked as intended' > %s/hello-world.rst", skeleton->skel_posts_folder_path);
  system(write_post_command);
  /************************/

  printf("[kevlar] Successfully created the skeleton; you can now run\n\n\tkevlar build\n\nto see your site in action ✨!\n");
}

void kevlar_handle_new_command(char folder_path[CONFIG_MAX_PATH_SIZE]) {
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
        "/config.ini",
        "/dist",
      };

      utl_prepend_str(folder_path, skel.skel_template_folder_path);
      utl_prepend_str(folder_path, skel.skel_posts_folder_path);
      utl_prepend_str(folder_path, skel.skel_config_file_path);

      kevlar_generate_new_skeleton(&skel);
      break; 
  }
}
