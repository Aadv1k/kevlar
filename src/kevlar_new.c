#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#if __has_include("windows.h")

#include <windows.h>

#endif

#include "../utils/utils.h"
#include "kevlar_errors.h"
#include "kevlar_handle_config.h"
#include "kevlar_new.h"

int kevlar_get_folder_status(const char folder_path[CONFIG_MAX_PATH_SIZE]) {
  DIR *dir_stream;
  struct dirent *dir_obj;

  dir_stream = opendir(folder_path);

  if (dir_stream == NULL)
    return folderNull;

  dir_obj = readdir(dir_stream);
  if (strcmp(dir_obj->d_name, ".") != 0 || strcmp(dir_obj->d_name, "..") != 0)
    return folderNonEmpty;

  return folderEmpty;
}

void kevlar_generate_new_skeleton(KevlarSkeleton *skeleton) {

  utl_mkdir_crossplatform(skeleton->skel_posts_folder_path);
  utl_mkdir_crossplatform(skeleton->skel_template_folder_path);
  kevlar_generate_skeleton_config(skeleton->skel_config_file_path);

  // TODO: Find some way for this to work on windows

  char clone_git_command[NEW_SYS_CMD_LEN];
  snprintf(clone_git_command, NEW_SYS_CMD_LEN * 2,
           "git clone https://github.com/aadv1k/kyudo \"%s/kyudo\" "
           ">/dev/null 2>&1",
           skeleton->skel_template_folder_path);

#if defined(_WIN32)
  system(clone_git_command);
#else

  if (system("git --version >/dev/null 2>&1") != 0) {
    kevlar_warn("couldn't find git on your system; not cloning any theme");
  } else {
    kevlar_ok("cloning theme into %s", skeleton->skel_template_folder_path);
    system(clone_git_command);
  }
#endif

  char default_rst_file_path[CONFIG_MAX_PATH_SIZE];
  strcpy(default_rst_file_path, skeleton->skel_posts_folder_path);
  strcat(default_rst_file_path, "/hello-world.rst");

  FILE *default_rst_file_buf;
  default_rst_file_buf = fopen(default_rst_file_path, "w");

  fprintf(default_rst_file_buf, "%s\n",
          "Welcome to kevlar!\n==================\n\nif you are seeing this, "
          "everything has worked as intended.");
  fclose(default_rst_file_buf);

  *(strchr(default_rst_file_path, '/')) = '\0';
  kevlar_ok("Skeleton was setup; you can now "
            "run\n\n\tcd %s && kevlar build\n\nto see your site in action ✨!",
            default_rst_file_path);
}

void kevlar_handle_new_command(char folder_path[CONFIG_MAX_PATH_SIZE]) {
  switch (kevlar_get_folder_status(folder_path)) {
  case folderNonEmpty:
    kevlar_err("folder \"%s\" already exists and is not empty!", folder_path);
    break;
  case folderNull:
    kevlar_warn("\"%s\" not found, creating new folder", folder_path);
    utl_mkdir_crossplatform(folder_path);
  // fall through
  case folderEmpty:;
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
