#ifndef UTILITIES_H
#define UTILITIES_H

typedef enum
{
    false, true
} bool;

#define MAX_STRING_LENGTH 80
#define MAX_SUBCOMMANDS 40 //TODO fai almeno dei controlli nel main
#define MAX_ARGUMENTS 50 //TODO fai almeno dei controlli

#define TOOL_NAME "xlogger"
#define TOOL_FOLDER TOOL_NAME"-ZZN5V7KH3H"
#define TOOL_VERSION_CODE "1.23.4 'copenhagen'" //TODO update version

#ifdef DEBUG
#include <stdio.h> //TODO is it needed?
#define DEBUG_PRINT( ... ) fprintf (stderr, __VA_ARGS__ )
#else
#define DEBUG_PRINT( ... )
#endif

/**
 * Print an help screen to stdout, then exit
 */
void printHelpAndExit();

/**
 * Print the current version of the tool to stdout, then exit
 */
void printVersionAndExit();

/**
 * Check is the string <b>str</b> starts with the prefix <b>*prefix</b>
 * @param str string to check
 * @param prefix prefix to match
 * @return true if <b>str</b> starts with the prefix <b>*prefix</b>, false otherwise
 */
bool strStartWith(const char *str, const char *prefix);

void sanityCheck();

#endif
