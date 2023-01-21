#ifndef KEVLAR_TEMPLATING_H
#define KEVLAR_TEMPLATING_H

#include <stdio.h>
#define TEMPLATE_MAX_TAG_SIZE 64

// REVIEW: May overflow
#define TEMPLATE_MAX_LINE_SIZE 10000
#define TEMPLATE_MAX_FILE_SIZE 100000

#include "kevlar_handle_config.h"
#include "kevlar_build.h"

void kevlar_parse_template(FILE *in_file_buffer, FILE *out_file_buffer, KevlarConfig *kev_config);

void kevlar_build_template(char *in_file_path, char *out_file_path, KevlarConfig *kev_config);

void kevlar_parse_post_from_template(char * html_file_path, char * template_file_path, char * config_file_path, ListingItem * item);

#endif
