#ifndef KEVLAR_ERRORS_H
#define KEVLAR_ERRORS_H

#define ERR_SIZE 256

#ifdef _WIN32

#define RED ""
#define GREEN ""
#define YELLOW ""
#define RESET ""

#else

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define RESET "\x1b[0m"

#endif

void kevlar_ok(const char *msg, ...);
void kevlar_err(const char *msg, ...);
void kevlar_warn(const char *msg, ...);

#endif
