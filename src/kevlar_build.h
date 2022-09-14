#ifndef KEVLAR_BUILD_H
#define KEVLAR_BUILD_H

#include "kevlar_handle_config.h"
#include "kevlar_new.h"

void kevlar_check_if_kevlar_proj(char file_path[MAX_FOLDER_PATH_SIZE], KevlarSkeleton skeleton);

void kevlar_handle_build_command(char file_path[MAX_FOLDER_PATH_SIZE], KevlarConfig * config);

#endif
