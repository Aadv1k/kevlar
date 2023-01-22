#ifndef KEVLAR_TEMPLATING_H
#define KEVLAR_TEMPLATING_H

#include <stdio.h>
#define TEMPLATE_MAX_TAG_SIZE 64

// REVIEW: May overflow
#define TEMPLATE_MAX_LINE_SIZE 10000
#define TEMPLATE_MAX_FILE_SIZE 100000

#include "kevlar_handle_config.h"
#include "kevlar_build.h"

void kevlar_parse_post_from_template(char * html_file_path, char * template_file_path, char * config_file_path, ListingItem * item);
void kevlar_generate_listing(char * entry_template_file_path, char * config_file_path, ListingItem * itemsList, char * target, int size);
void kevlar_generate_index_from_template( char * dist_path, char * template_folder_path, char * config_path, ListingItem * itemsList, int size);

#endif
