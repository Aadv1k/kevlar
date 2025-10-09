#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#if __has_include("windows.h")

#include <windows.h>

#endif

#include "utils.h"
#include "kevlar_errors.h"
#include "kevlar_build.h"
#include "kevlar_handle_config.h"
#include "kevlar_new.h"

int kevlar_get_folder_status(const char * folder_path) {
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

void kevlar_generate_new_post(const char * folder_path, char * post_title) {
  char file_name[CONFIG_MAX_PATH_SIZE] = "";
  char * input_tmp = strdup(post_title);
  char output[CONFIG_MAX_PATH_SIZE];

  utl_spaces_to_dash_case(input_tmp, output);
  strcpy(file_name, folder_path);
  strcat(file_name, "/");
  strcat(file_name, output);
  strcat(file_name, ".md");

  kevlar_ok("generating \"%s\"...", file_name);

  size_t fileNum = kevlar_count_files_in_folder(folder_path, "md");
  FILE * md_file_buf = fopen(file_name, "w");
  if (!md_file_buf) kevlar_err("something went wrong while generating %s", file_name);

  char * time_str = utl_get_date_time_unsafe();

  fprintf(md_file_buf, "Title=%s\n", post_title);
  fprintf(md_file_buf, "Date=%s\n", time_str);
  fprintf(md_file_buf, "Order=%lu\n", fileNum+1);
  fputs("\n", md_file_buf);
  fputs("Goodness awaits...", md_file_buf);
}

void kevlar_generate_new_skeleton(KevlarSkeleton *skeleton) {
  utl_mkdir_crossplatform(skeleton->skel_posts_folder_path);
  utl_mkdir_crossplatform(skeleton->skel_template_folder_path);
  kevlar_generate_skeleton_config(skeleton->skel_config_file_path);

  // TODO: Find some way for this to work on windows

  char clone_git_command[NEW_SYS_CMD_LEN] = "git clone https://github.com/aadv1k/listed-kevlar-theme ";
  strcat(clone_git_command, skeleton->skel_template_folder_path);
  strcat(clone_git_command, "/listed-kevlar-theme");
  strcat(clone_git_command, " >/dev/null 2>&1");

#if defined(_WIN32)
  system(clone_git_command);
#else

  if (system("git --version >/dev/null 2>&1") != 0) {
    kevlar_warn("couldn't find git on your system; continuing without a theme");
  } else {
    kevlar_ok("cloning theme into %s", skeleton->skel_template_folder_path);
    if (system(clone_git_command) == -1) {
      kevlar_warn("the command `%s` failed; continuing without a theme", clone_git_command);
    };
  }
#endif

  char default_md_file_path[CONFIG_MAX_PATH_SIZE];
  strcpy(default_md_file_path, skeleton->skel_posts_folder_path);
  strcat(default_md_file_path, "/hello-world.md");

  FILE *default_md_file_buf;
  default_md_file_buf = fopen(default_md_file_path, "w");
  char * time_str = utl_get_date_time_unsafe();

  fputs("Title=My first post\n", default_md_file_buf);
  fprintf(default_md_file_buf, "Date=%s\n", time_str);
  fputs("Order=0\n", default_md_file_buf);
  fputs("\n", default_md_file_buf);
  fputs("# Welcome to kevlar!\n", default_md_file_buf);
  fputs("if you are seeing this, everything has worked as intended.\n", default_md_file_buf);

  free(time_str);

  fclose(default_md_file_buf);

  *(strchr(default_md_file_path, '/')) = '\0';
  kevlar_ok("Skeleton was setup; you can now "
            "run\n\n\tcd %s && kevlar build\n\nto see your site in action ✨!",
            default_md_file_path);
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
