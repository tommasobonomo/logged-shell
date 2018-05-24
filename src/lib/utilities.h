#ifndef UTILITIES_H
#define UTILITIES_H

typedef enum
{
    false, true
} bool;

#define MAX_STRING_LENGHT 80
#define MAX_SUBCOMMANDS 40 //TODO fai almeno dei controlli nel main
#define MAX_ARGUMENTS 50 //TODO fai almeno dei controlli

#define TOOL_NAME "xlogger"

#ifdef DEBUG
#include <stdio.h> //TODO is it needed?
#define DEBUG_PRINT( ... ) fprintf (stderr, __VA_ARGS__ )
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

void sanityCheck();

#endif