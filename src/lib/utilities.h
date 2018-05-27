#ifndef UTILITIES_H
#define UTILITIES_H

typedef enum
{
    false, true
} bool;

#define MAX_STRING_LENGTH 90
#define MAX_SUBCOMMANDS 40
#define MAX_ARGUMENTS 50 //TODO fai almeno dei controlli

#define TOOL_NAME "xlogger"
#define TOOL_FOLDER TOOL_NAME"-ZZN5V7KH3H"
#define TOOL_VERSION_CODE "1.23.4 'copenhagen'" //TODO update version

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

/**
 * Internal errors
 */
#define EXIT_PARAMETER_FAILURE 2
#define EXIT_CMD_NOT_FOUND_FAILURE 127
#define EXIT_FATAL_SIGNAL 128

#ifdef DEBUG
#include <stdio.h>
#define DEBUG_PRINT( ... ) fprintf (stderr, __VA_ARGS__ )
#else
#define DEBUG_PRINT( ... )
#endif

/**
 * Print an help screen to stdout, then exit
 */
void printHelpAndExit(int status);

/**
 * Print the current version of the tool to stdout, then exit
 */
void printVersionAndExit();

/**
 * Show default log to terminal if custom path is not specified; then exits
 */
void showLogAndExit(const char *path);

/**
 * Check is the string <b>str</b> starts with the prefix <b>*prefix</b>
 * @param str string to check
 * @param prefix prefix to match
 * @return true if <b>str</b> starts with the prefix <b>*prefix</b>, false otherwise
 */
bool strStartWith(const char *str, const char *prefix);

void sanityCheck();

#endif
