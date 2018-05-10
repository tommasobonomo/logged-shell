#ifndef UTILITIES_H
#define UTILITIES_H

typedef enum
{
    false, true
} bool;

#define CMD_STRING_LENGHT_MAX 500
#define PATH_MAX 4095
#define MAX_SUBCOMMANDS 80 //TODO non bello, fai almeno dei controlli nel main

#ifdef DEBUG
#include <stdio.h> //TODO is it needed?
#define DEBUG_PRINT( ... ) printf ( __VA_ARGS__ )
#else
#define DEBUG_PRINT( ... )
#endif

/**
 * Print an help screen to stdout
 */
void printHelp();

/**
 * Check is the string <b>str</b> starts with the prefix <b>*prefix</b>
 * @param str string to check
 * @param prefix prefix to match
 * @return true if <b>str</b> starts with the prefix <b>*prefix</b>, false otherwise
 */
bool strStartWith(const char *str, const char *prefix);

#endif