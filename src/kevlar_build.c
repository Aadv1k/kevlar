#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "kevlar_build.h"
#include "kevlar_handle_config.h"
#include "kevlar_new.h"

#include "../utils/utils.h"
#include "kevlar_errors.h"
#include "kevlar_md_to_html.h"
#include "kevlar_rst_to_html.h"
#include "kevlar_templating.h"

void kevlar_copy_assets(const char * src, const char * dest) {

  if (kevlar_get_folder_status(src) == folderNull || kevlar_get_folder_status(src) == folderEmpty) {
    kevlar_warn("asset path \"%s\" is either empty or doesn't exist", src);
    return;
  } 
  
  DIR * in_dir_stream;

  in_dir_stream = opendir(src);

  struct dirent * in_dir_obj;

  while ((in_dir_obj = readdir(in_dir_stream))) {
    if (in_dir_obj->d_name[0] == '.') {
      continue;
    }

    char out_file_path[CONFIG_MAX_PATH_SIZE];
    snprintf(out_file_path, CONFIG_MAX_PATH_SIZE, "%s/%s", dest, in_dir_obj->d_name);

    char in_file_path[CONFIG_MAX_PATH_SIZE];
    snprintf(in_file_path, CONFIG_MAX_PATH_SIZE, "%s/%s", src, in_dir_obj->d_name);

    FILE * in_file_buf = fopen(in_file_path, "rb");
    FILE * out_file_buf = fopen(out_file_path, "wb");

    if (!out_file_buf) kevlar_err("[%s] couldn't open %s", __FILE__, out_file_path);
    if (!in_file_buf) kevlar_err("[%s] couldn't open %s", __FILE__, in_file_path);

    int in_char;
    while (( in_char = fgetc(in_file_buf)) != EOF) {
      fputc(in_char, out_file_buf);
    }
  }

  closedir(in_dir_stream);
}

void kevlar_check_if_theme_valid(const char theme_path[CONFIG_MAX_PATH_SIZE]) {
  enum FolderStatus;

  char full_theme_path[CONFIG_MAX_PATH_SIZE];
  strcpy(full_theme_path, "templates/");
  strcat(full_theme_path, theme_path);

  if (kevlar_get_folder_status(full_theme_path) == folderNull) {
    kevlar_err("couldn't find theme %s", theme_path);
  } else if (kevlar_get_folder_status(full_theme_path) == folderEmpty) {
    kevlar_err("the theme %s seems to be invalid", theme_path);
    exit(1);
  }
}

void kevlar_generate_listings(char dist_path[CONFIG_MAX_PATH_SIZE], KevlarConfig *kev_config) {

  DIR *dir_buf;
  struct dirent *dir_item;

  if ((dir_buf = opendir(dist_path)) == NULL) {
    kevlar_err("something went wrong while opening %s to generate listings", dist_path);
  }

  while ((dir_item = readdir(dir_buf)) != NULL) {
    if (dir_item->d_name[0] != '.') {

      char file_name[999];
      char out_file_name[999];

      strcpy(file_name, dir_item->d_name);

      file_name[
        strlen(file_name) - strlen(strrchr(file_name, '.'))
      ] = '\0';

      dir_item->d_name[
        strlen(dir_item->d_name) - strlen(strrchr(dir_item->d_name, '.'))
      ] = '\0';

      char html_li_link[CONFIG_MAX_FILE_LINE_SIZE];

      snprintf(html_li_link, CONFIG_MAX_FILE_LINE_SIZE, "<li><a href=\"./%s.html\">%s</a></li>\n", dir_item->d_name, utl_camel_case_to_spaces(file_name, out_file_name));

      strcat(kev_config->configListing, html_li_link);
    }
  }
  closedir(dir_buf);
}

void kevlar_parse_md_from_folder(char folder_path[CONFIG_MAX_PATH_SIZE],
                                 char out_folder_path[CONFIG_MAX_PATH_SIZE], char *md_loader,
                                 KevlarConfig *kev_config) {

  enum FolderStatus;

  DIR *dir_buffer;
  struct dirent *dir_item;

  if (kevlar_get_folder_status(folder_path) == folderNull) {
    kevlar_err("couldn't open %s, it might not be a folder", folder_path);
  }

  dir_buffer = opendir(folder_path);

  utl_mkdir_crossplatform(out_folder_path);

  int i = 0;
  while ((dir_item = readdir(dir_buffer)) != NULL) {

    if (strcmp(strrchr(dir_item->d_name, '.'), ".md") == 0) {

      char md_file_path[CONFIG_MAX_PATH_SIZE] = "";
      char html_file_path[CONFIG_MAX_PATH_SIZE] = "";
      char system_command[BUILD_MAX_CMD_SIZE] = "";

      strcpy(md_file_path, dir_item->d_name);

      strcpy(html_file_path, md_file_path);

      strcpy(md_file_path, "./posts");

      strcat(md_file_path, "/");
      strcat(md_file_path, dir_item->d_name);

      html_file_path[strlen(html_file_path) - 3] = '\0';
      strcat(html_file_path, ".html");
      utl_prepend(html_file_path, "/");
      utl_prepend(html_file_path, out_folder_path);

      if (strlen(kev_config->configMarkdownLoader) != 0) {
        snprintf(system_command, BUILD_MAX_CMD_SIZE, "%s %s %s", md_loader, md_file_path,
                 html_file_path);
        if (system(system_command) == -1) {
          kevlar_warn("command `%s` did not work; continuing without parsing markdown file",
                      system_command);
        }
      } else {
        md_parse(md_file_path, html_file_path);
      }

      FILE *html_file_buf = fopen(html_file_path, "r");

      if (html_file_buf == NULL) {
        kevlar_err("was unable to open html files for further processing, maybe the system command "
                   "went wrong?");
      }

      char contents[TEMPLATE_MAX_FILE_SIZE] = "";
      char html_file_line[TEMPLATE_MAX_LINE_SIZE];


      while ((fgets(html_file_line, TEMPLATE_MAX_LINE_SIZE, html_file_buf)) != NULL) {
        strcat(contents, html_file_line);
      }

      fclose(html_file_buf);

      strcpy(kev_config->configHtmlContents, contents);

      kev_config->configListing[0] = '\0';
      kevlar_generate_listings("./posts", kev_config);

      kevlar_build_template(kev_config->configPostPath, html_file_path, kev_config);
      i++;
    }
  }

  if (i == 0) {
    kevlar_warn("found no .md files in %s", folder_path);
  }
}

void kevlar_parse_rst_from_folder(char folder_path[CONFIG_MAX_PATH_SIZE],
                                  char out_folder_path[CONFIG_MAX_PATH_SIZE], char *rst_loader,
                                  KevlarConfig *kev_config) {
  enum FolderStatus;

  DIR *dir_buffer;
  struct dirent *dir_item;

  if (kevlar_get_folder_status(folder_path) == folderNull) {
    kevlar_err("couldn't open %s, it might not be a folder", folder_path);
  }

  dir_buffer = opendir(folder_path);

  utl_mkdir_crossplatform(out_folder_path);

  int i = 0;
  while ((dir_item = readdir(dir_buffer)) != NULL) {

    if (strcmp(strrchr(dir_item->d_name, '.'), ".rst") == 0) {

      char rst_file_path[CONFIG_MAX_PATH_SIZE];
      char html_file_path[CONFIG_MAX_PATH_SIZE];
      char system_command[BUILD_MAX_CMD_SIZE];

      strcpy(rst_file_path, dir_item->d_name);

      strcpy(html_file_path, rst_file_path);
      utl_prepend(rst_file_path, folder_path);

      html_file_path[strlen(html_file_path) - 4] = '\0';
      strcat(html_file_path, ".html");

      utl_prepend(html_file_path, "/");
      utl_prepend(html_file_path, out_folder_path);

      if (strlen(kev_config->configRstLoader) != 0) {
        snprintf(system_command, BUILD_MAX_CMD_SIZE, "%s %s %s", rst_loader, rst_file_path,
                 html_file_path);
        if (system(system_command) == -1) {
          kevlar_warn("command `%s` did not work; continuing without parsing rst file",
                      system_command);
        }
      } else {
        rst_parse(rst_file_path, html_file_path);
      }

      FILE *html_file_buf = fopen(html_file_path, "r");

      if (html_file_buf == NULL) {
        kevlar_err("was unable to open html files for further processing, maybe the system command "
                   "went wrong?");
      }

      char contents[TEMPLATE_MAX_FILE_SIZE] = "";
      char html_file_line[TEMPLATE_MAX_LINE_SIZE];

      while ((fgets(html_file_line, TEMPLATE_MAX_LINE_SIZE, html_file_buf)) != NULL) {
        strcat(contents, html_file_line);
      }

      fclose(html_file_buf);

      strcpy(kev_config->configHtmlContents, contents);

      kevlar_build_template(kev_config->configPostPath, html_file_path, kev_config);
      i++;
    }
  }

  if (i == 0) {
    kevlar_warn("found no .rst files in %s", folder_path);
  }
}

void kevlar_check_if_kevlar_proj(const char folder_path[CONFIG_MAX_PATH_SIZE],
                                 KevlarSkeleton *skeleton) {
  enum FolderStatus;

  if (kevlar_get_folder_status(folder_path) == folderNull) {
    kevlar_err("ran into a problem while opening %s; it may not exist", folder_path);
  }

  if ((kevlar_get_folder_status(skeleton->skel_posts_folder_path) == folderNull) ||
      (kevlar_get_folder_status(skeleton->skel_template_folder_path) == folderNull) ||
      (kevlar_get_folder_status(skeleton->skel_config_file_path) != folderNull)) {
    kevlar_err("this doesn't seem to be a kevlar project, try running "
               "kevlar help for info",
               folder_path);
  }
}

void kevlar_handle_build_command(const char file_path[CONFIG_MAX_PATH_SIZE]) {
  KevlarSkeleton skel = {"templates/", "posts/", "config.ini", "dist/"};
  kevlar_check_if_kevlar_proj(file_path, &skel);

  utl_mkdir_crossplatform("./dist");

  char config_path[CONFIG_MAX_PATH_SIZE];
  snprintf(config_path, CONFIG_MAX_PATH_SIZE, "%s%s", file_path, "/config.ini");

  KevlarConfig kev_config;
  kevlar_load_config(config_path, &kev_config);

  char posts_path[CONFIG_MAX_PATH_SIZE];
  snprintf(posts_path, CONFIG_MAX_PATH_SIZE, "%s%s", file_path, "/posts/");

  kevlar_check_if_theme_valid(kev_config.configTheme);

  kevlar_parse_rst_from_folder(posts_path, "./dist", kev_config.configRstLoader, &kev_config);
  kevlar_parse_md_from_folder(posts_path, "./dist", kev_config.configMarkdownLoader, &kev_config);

  kevlar_copy_assets("./assets", "./dist");

  kev_config.configListing[0] = '\0';
  kevlar_generate_listings("./posts", &kev_config);

  kevlar_build_template(kev_config.configIndexPath, "./dist/index.html", &kev_config);
  kevlar_ok("Built the site using theme %s at ./dist/index.html", kev_config.configTheme);
}
