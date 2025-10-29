#include "kevlar_errors.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void kevlar_warn(const char *msg, ...) {
    char buffer[ERR_SIZE];
    va_list args;
    va_start(args, msg);
    vsnprintf(buffer, ERR_SIZE - 1, msg, args);

    fprintf(stderr, YELLOW "WARN: %s\n" RESET, buffer);
    va_end(args);
}

void kevlar_err(const char *msg, ...) {
    char buffer[ERR_SIZE];
    va_list args;
    va_start(args, msg);
    vsnprintf(buffer, ERR_SIZE - 1, msg, args);
    va_end(args);

    fprintf(stderr, RED "ERROR: " RESET "%s\n", buffer);
    exit(1);
}

void kevlar_ok(const char *msg, ...) {
    char buffer[ERR_SIZE];
    va_list args;
    va_start(args, msg);
    vsnprintf(buffer, ERR_SIZE - 1, msg, args);
    va_end(args);

    fprintf(stdout, GREEN "SUCCESS: " RESET "%s\n", buffer);
}
