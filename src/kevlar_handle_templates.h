#include <stdio.h>
#define TEMPLATE_MAX_TAG_SIZE 64
#define TEMPLATE_MAX_LINE_SIZE 224

#include "kevlar_handle_config.h"

/** 
 * Find a char substr[] in the char line[] and replace it with char content[] and write it to the out_file_buffer
 */
void kevlar_write_html_string(char line[], char substr[], char content[], FILE * out_file_buffer);

/**
 * Parse an html template and fill in the configuration otpions from the KevlarConfig
 */
void kevlar_parse_template(char file_path[CONFIG_MAX_PATH_SIZE], char out_file_path[CONFIG_MAX_PATH_SIZE], KevlarConfig *kev_config);

