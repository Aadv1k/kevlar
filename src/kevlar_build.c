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

#include "kevlar_errors.h"
#include "kevlar_md_to_html.h"
#include "kevlar_templating.h"
#include "utils.h"

// WARN: Deprecated
#include "kevlar_rst_to_html.h"

#define FILE_OPTS 3

size_t kevlar_count_files_in_folder(const char *folder_path, const char *filetype) {
    DIR *dir = opendir(folder_path);
    struct dirent *dir_itm;
    size_t itm_count = 0;

    while ((dir_itm = readdir(dir)) != NULL) {
        if (dir_itm->d_name[0] == '.')
            continue;
        char *rev = strchr(dir_itm->d_name, '.') + 1;
        if (kevlar_get_folder_status(dir_itm->d_name) != folderNull)
            continue;
        if (strcmp(rev, filetype) == 0)
            itm_count++;
    }
    return itm_count;
}

void kevlar_copy_assets(const char *src, const char *dest) {
    if (kevlar_get_folder_status(src) == folderNull ||
        kevlar_get_folder_status(src) == folderEmpty) {
        kevlar_warn("asset path \"%s\" is either empty or doesn't exist", src);
        return;
    }

    DIR *in_dir_stream;

    in_dir_stream = opendir(src);

    struct dirent *in_dir_obj;

    while ((in_dir_obj = readdir(in_dir_stream))) {
        if (in_dir_obj->d_name[0] == '.') {
            continue;
        }

        char out_file_path[CONFIG_MAX_PATH_SIZE];
        snprintf(out_file_path, CONFIG_MAX_PATH_SIZE, "%s/%s", dest, in_dir_obj->d_name);

        char in_file_path[CONFIG_MAX_PATH_SIZE];
        snprintf(in_file_path, CONFIG_MAX_PATH_SIZE, "%s/%s", src, in_dir_obj->d_name);

        FILE *in_file_buf = fopen(in_file_path, "rb");
        FILE *out_file_buf = fopen(out_file_path, "wb");

        if (!out_file_buf)
            kevlar_err("[%s] couldn't open %s", __FILE__, out_file_path);
        if (!in_file_buf)
            kevlar_err("[%s] couldn't open %s", __FILE__, in_file_path);

        int in_char;
        while ((in_char = fgetc(in_file_buf)) != EOF) {
            fputc(in_char, out_file_buf);
        }
    }
    closedir(in_dir_stream);
}

// CHECKS, ensure nothing goes wrong
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

// https://stackoverflow.com/questions/13372688/sorting-members-of-structure-array
void bsortDesc(ListingItem *itemsList, int s) {
    int i, j;
    ListingItem temp;

    for (i = 0; i < s - 1; i++) {
        for (j = 0; j < (s - 1 - i); j++) {
            if (itemsList[j].lOrder < itemsList[j + 1].lOrder) {
                temp = itemsList[j];
                itemsList[j] = itemsList[j + 1];
                itemsList[j + 1] = temp;
            }
        }
    }
}

void kevlar_parse_md_from_folder(char *folder_path, char *dist_path, char *template_path,
                                 ListingItem *itemsList) {
    DIR *folder_buf = opendir(folder_path);
    struct dirent *dir_item;

    int post_count = 0;
    while ((dir_item = readdir(folder_buf)) != NULL) {
        if (dir_item->d_name[0] == '.')
            continue;

        char in_fp[CONFIG_MAX_PATH_SIZE], out_fp[CONFIG_MAX_PATH_SIZE];

        sprintf(in_fp, "%s/%s", folder_path, dir_item->d_name);
        dir_item->d_name[strlen(dir_item->d_name) - 3] = '\0';
        sprintf(out_fp, "%s/%s.html", dist_path, dir_item->d_name);

        kevlar_get_opt_from_config(in_fp, "date", itemsList[post_count].lDate);
        kevlar_get_opt_from_config(in_fp, "title", itemsList[post_count].lTitle);

        char order[TEMPLATE_MAX_TAG_SIZE];
        kevlar_get_opt_from_config(in_fp, "order", order);

        itemsList[post_count].lOrder = atoi(order);
        strcpy(itemsList[post_count].lPath, out_fp);

        md_parse(in_fp, out_fp, 3);
        FILE *out_file = fopen(out_fp, "r");

        fseek(out_file, 0, SEEK_END);
        int out_file_length = ftell(out_file);
        rewind(out_file);

        char *buf = malloc(out_file_length + 1);
        if (!fread(buf, out_file_length, 1, out_file))
            kevlar_err("was unable to read %s", out_file);

        strcpy(itemsList[post_count].lContent, buf);
        buf[out_file_length] = '\0';
        free(buf);

        char postTemplatePath[CONFIG_MAX_PATH_SIZE];
        strcpy(postTemplatePath, template_path);
        strcat(postTemplatePath, "/post.html");

        kevlar_parse_post_from_template(out_fp, postTemplatePath, "./config.ini",
                                        &itemsList[post_count]);
        fclose(out_file);
        post_count++;
    }
}

void kevlar_handle_build_command(const char *file_path) {
    KevlarSkeleton skel = {"templates/", "posts/", "config.ini", "dist/"};
    kevlar_check_if_kevlar_proj(file_path, &skel);
    char cTheme[CONFIG_MAX_PATH_SIZE];
    kevlar_get_opt_from_config("./config.ini", "theme", cTheme);
    char themePath[CONFIG_MAX_PATH_SIZE] = "./templates/";
    strcat(themePath, cTheme);
    kevlar_check_if_theme_valid(cTheme);

    utl_mkdir_crossplatform("dist");

    size_t md_count = kevlar_count_files_in_folder("./posts", "md");
    ListingItem *itemsList = malloc(md_count * sizeof(ListingItem));
    kevlar_parse_md_from_folder("./posts", "./dist", themePath, itemsList);

    bsortDesc(itemsList, md_count);

    kevlar_generate_index_from_template("./dist/index.html", themePath, "./config.ini", itemsList,
                                        md_count);

    free(itemsList);

    // kevlar_build_template(kev_config.configIndexPath, "./dist/index.html", &kev_config);
    // kevlar_ok("Built the site using theme %s at ./dist/index.html", kev_config.configTheme);
}
