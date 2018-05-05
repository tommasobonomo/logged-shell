#ifndef UTILITIES_H
#define UTILITIES_H

typedef enum
{
    false, true
} bool;

#define CMD_STRING_LENGHT_MAX 500
#define PATH_MAX 4095

#ifdef DEBUG
#include <stdio.h> //TODO is it needed?
#define DEBUG_PRINT( ... ) printf ( __VA_ARGS__ )
#else
#define DEBUG_PRINT( ... )
#endif

void printHelp();
bool strStartWith(const char *str, const char *prefix);

#endif