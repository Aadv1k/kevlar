#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "kevlar_build.h"
#include "kevlar_handle_config.h"
#include "kevlar_new.h"

#include "../utils/utils.h"

// TODO: add a output folder param, since right now it odes so in the base directory 
void kevlar_parse_rst_from_folder(char folder_path[CONFIG_MAX_PATH_SIZE], char out_folder_path[CONFIG_MAX_PATH_SIZE], char *rst_loader) {
  enum FolderStatus;

  DIR * dir_buffer;
  struct dirent *dir_item;

  if (kevlar_get_folder_status(folder_path) == folderNull) {
    fprintf(stderr, "[kevlar] couldn't open %s, it might not be a folder\n", folder_path);
    exit(1);
  }

  // We don't need to wrap this with a guard since the above already handles it
  dir_buffer = opendir(folder_path);

  
  // TODO: implement this? 
  //if (kevlar_get_folder_status(out_folder_path) == folderNonEmpty) { fprintf(stderr, "[kevlar] cannot output .html files to %s as it is not empty\n", out_folder_path); exit(1); }

  mkdir(out_folder_path, FOLDER_ALL_PERMS);

  int i = 0;
  while ((dir_item = readdir(dir_buffer)) != NULL) {
    if (strcmp(utl_strchrev(dir_item->d_name, '.'), ".rst") == 0) {
      char rst_file[CONFIG_MAX_PATH_SIZE];
      char html_file[CONFIG_MAX_PATH_SIZE];
      char system_command[BUILD_MAX_CMD_SIZE];

      strcpy(rst_file, dir_item->d_name);
      strcpy(html_file, rst_file);
      utl_prepend(rst_file, folder_path);
      
      html_file[strlen(html_file)-4] = '\0';
      strcat(html_file, ".html");

      utl_prepend(html_file, "/");
      utl_prepend(html_file, out_folder_path);

      sprintf(system_command, "%s %s %s", rst_loader, rst_file, html_file);
      //fprintf(stdout, "%s %s %s\n", rst_loader, rst_file, html_file);
      system(system_command);
      i++;
    }
  }

  if (i == 0) {
    fprintf(stderr, "[kevlar] no .rst files were found in %s\n", folder_path);
  }
}

// TODO: use the "stat" command instead of whatever this is 
void kevlar_check_if_kevlar_proj(char folder_path[MAX_FOLDER_PATH_SIZE], KevlarSkeleton *skeleton) {
  enum FolderStatus;

  if (kevlar_get_folder_status(folder_path) == folderNull) {
    fprintf(stderr, "[kevlar] ran into a problem while opening %s; it may not exist\n", folder_path);
    exit(1);
  }

  if (
    (kevlar_get_folder_status(skeleton->skel_posts_folder_path) == folderNull) ||
    (kevlar_get_folder_status(skeleton->skel_template_folder_path) == folderNull) ||
    (kevlar_get_folder_status(skeleton->skel_config_file_path) != folderNull)
  )  {
    fprintf(stderr, "[kevlar] %s doesn't seem to be a kevlar project, try running kevlar new for a new project\n", folder_path);
    exit(1);
  }
}

void kevlar_handle_build_command(char file_path[MAX_FOLDER_PATH_SIZE]) {

  KevlarSkeleton skel = { "templates/", "posts/", "config.ini", "dist/" };

  kevlar_check_if_kevlar_proj(file_path, &skel);

  DIR * dir_stream; 
  struct dirent *dir_itm;

  mkdir("./dist", FOLDER_ALL_PERMS);

  char config_path[] = "/config.ini";
  utl_prepend(config_path, file_path);

  KevlarConfig kev_config;
  kevlar_load_config(config_path, &kev_config);

  char posts_path[] = "/posts/";
  utl_prepend(posts_path, file_path);

  kevlar_parse_rst_from_folder(posts_path, "./dist", kev_config.configRstLoader);
}
