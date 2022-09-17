#ifndef KEVLAR_BUILD_H
#define KEVLAR_BUILD_H

#include "kevlar_handle_config.h"
#include "kevlar_new.h"

#define BUILD_MAX_CMD_SIZE 200

void kevlar_check_if_kevlar_proj(char folder_path[MAX_FOLDER_PATH_SIZE], KevlarSkeleton *skeleton);

void kevlar_handle_build_command(char folder_path[MAX_FOLDER_PATH_SIZE]);

#endif
