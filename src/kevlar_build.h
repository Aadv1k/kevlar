#ifndef KEVLAR_BUILD_H
#define KEVLAR_BUILD_H

#include "kevlar_handle_config.h"
#include "kevlar_new.h"

#define BUILD_MAX_CMD_SIZE 200

void kevlar_check_if_theme_valid(char theme_path[CONFIG_MAX_PATH_SIZE]);

void evlar_check_if_kevlar_proj(char folder_path[MAX_FOLDER_PATH_SIZE], KevlarSkeleton *skeleton);

/** 
 * Reads all `.html` files in dist_path[] and generate links sorrounded with <li>, and put them inside kev_config->configListing
 */
void kevlar_generate_listings(char dist_path[CONFIG_MAX_PATH_SIZE], KevlarConfig *kev_config);

/** 
 * Converts all `.rst` files from folder_path[], convert them to html in out_folder_path[] using *rst_loader and load each file as a `post.html` template
 */
void kevlar_parse_rst_from_folder(char folder_path[CONFIG_MAX_PATH_SIZE], char out_folder_path[CONFIG_MAX_PATH_SIZE], char *rst_loader, KevlarConfig * kev_config);

void kevlar_handle_build_command(char folder_path[MAX_FOLDER_PATH_SIZE]);

#endif
