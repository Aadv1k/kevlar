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
#include "kevlar_rst_to_html.h"
#include "kevlar_templating.h"

void kevlar_check_if_theme_valid(const char theme_path[CONFIG_MAX_PATH_SIZE]) {
    enum FolderStatus;

    char full_theme_path[CONFIG_MAX_PATH_SIZE];
    strcpy(full_theme_path, "templates/");
    strcat(full_theme_path, theme_path);

    if (kevlar_get_folder_status(full_theme_path) == folderNull) {
        fprintf(stderr, "[kevlar] couldn't find theme %s\n", theme_path);
        exit(1);
    } else if (kevlar_get_folder_status(full_theme_path) == folderEmpty) {
        fprintf(stderr, "[kevlar] the theme %s seems to be invalid\n",
                theme_path);
        exit(1);
    }
}

void kevlar_generate_listings(char dist_path[CONFIG_MAX_PATH_SIZE],
                              KevlarConfig *kev_config) {

    DIR *dir_buf;
    struct dirent *dir_item;

    if ((dir_buf = opendir(dist_path)) == NULL) {
        fprintf(stderr, "[kevlar] something went wrong while opening ./dist/ "
                        "to generate listings");
        exit(1);
    }

    while ((dir_item = readdir(dir_buf)) != NULL) {
        if (strcmp(utl_strchrev(dir_item->d_name, '.'), ".html") == 0 &&
            strcmp(dir_item->d_name, "index.html") != 0) {

            char *tmp = strdup(dir_item->d_name);
            dir_item->d_name[strlen(dir_item->d_name) - 5] = '\0';

            char html_li_link[CONFIG_MAX_FILE_LINE_SIZE];
            snprintf(html_li_link,
                     CONFIG_MAX_FILE_LINE_SIZE + sizeof(dir_item->d_name),
                     "<li><a href=\"./%s\">%s</a></li>\n", tmp,
                     dir_item->d_name);
            strcat(kev_config->configListing, html_li_link);
            free(tmp);
        }
    }
    closedir(dir_buf);
}

void kevlar_parse_rst_from_folder(char folder_path[CONFIG_MAX_PATH_SIZE],
                                  char out_folder_path[CONFIG_MAX_PATH_SIZE],
                                  char *rst_loader, KevlarConfig *kev_config) {
    enum FolderStatus;

    DIR *dir_buffer;
    struct dirent *dir_item;

    if (kevlar_get_folder_status(folder_path) == folderNull) {
        fprintf(stderr, "[kevlar] couldn't open %s, it might not be a folder\n",
                folder_path);
        exit(1);
    }

    dir_buffer = opendir(folder_path);

    utl_mkdir_crossplatform(out_folder_path);

    int i = 0;
    while ((dir_item = readdir(dir_buffer)) != NULL) {

        if (strcmp(utl_strchrev(dir_item->d_name, '.'), ".rst") == 0) {

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
                snprintf(system_command, BUILD_MAX_CMD_SIZE * 3, "%s %s %s",
                         rst_loader, rst_file_path, html_file_path);
                system(system_command);
            } else {
                rst_parse(rst_file_path, html_file_path);
            }

            FILE *html_file_buf = fopen(html_file_path, "r");

            if (html_file_buf == NULL) {
                fprintf(stderr,
                        "[kevlar] was unable to open html files for further "
                        "processing, maybe the system command went wrong?\n");
                exit(1);
            }

            char contents[TEMPLATE_MAX_FILE_SIZE] = "";
            char html_file_line[TEMPLATE_MAX_LINE_SIZE];

            while ((fgets(html_file_line, TEMPLATE_MAX_LINE_SIZE,
                          html_file_buf)) != NULL) {
                strcat(contents, html_file_line);
            }

            fclose(html_file_buf);

            strcpy(kev_config->configHtmlContents, contents);

            kevlar_build_template(kev_config->configPostPath, html_file_path,
                                  kev_config);

            i++;
        }
    }

    if (i == 0) {
        fprintf(stderr, "[kevlar] found no .rst files in %s\n", folder_path);
        return;
    }

    kevlar_generate_listings("./dist", kev_config);
}

void kevlar_check_if_kevlar_proj(const char folder_path[CONFIG_MAX_PATH_SIZE],
                                 KevlarSkeleton *skeleton) {
    enum FolderStatus;

    if (kevlar_get_folder_status(folder_path) == folderNull) {
        fprintf(
            stderr,
            "[kevlar] ran into a problem while opening %s; it may not exist\n",
            folder_path);
        exit(1);
    }

    if ((kevlar_get_folder_status(skeleton->skel_posts_folder_path) ==
         folderNull) ||
        (kevlar_get_folder_status(skeleton->skel_template_folder_path) ==
         folderNull) ||
        (kevlar_get_folder_status(skeleton->skel_config_file_path) !=
         folderNull)) {
        fprintf(stderr,
                "[kevlar] %s doesn't seem to be a kevlar project, try running "
                "kevlar help for info\n",
                folder_path);
        exit(1);
    }
}

void kevlar_handle_build_command(const char file_path[CONFIG_MAX_PATH_SIZE]) {
    KevlarSkeleton skel = {"templates/", "posts/", "config.ini", "dist/"};
    kevlar_check_if_kevlar_proj(file_path, &skel);

    utl_mkdir_crossplatform("./dist");

    char config_path[CONFIG_MAX_PATH_SIZE];
    snprintf(config_path, CONFIG_MAX_PATH_SIZE, "%s%s", file_path,
             "/config.ini");

    KevlarConfig kev_config;
    kevlar_load_config(config_path, &kev_config);

    char posts_path[CONFIG_MAX_PATH_SIZE];
    snprintf(posts_path, CONFIG_MAX_PATH_SIZE, "%s%s", file_path, "/posts/");

    char *const p_configTheme = kev_config.configTheme;
    kevlar_check_if_theme_valid(p_configTheme);

    kevlar_parse_rst_from_folder(posts_path, "./dist",
                                 kev_config.configRstLoader, &kev_config);

    kevlar_build_template(kev_config.configIndexPath, "./dist/index.html",
                          &kev_config);
}
