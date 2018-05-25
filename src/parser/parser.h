#ifndef PARSER_H
#define PARSER_H

#include "../lib/commands.h"

// NORMAL ARGUMENTS
#define ARG_LOGPATH "p"
#define ARG_LOGFORMAT "f"

#define ARG_OUTSCREEN "o"
#define ARG_OUTFILEAPP "a"
#define ARG_OUTFILEOVER "x"

#define ARG_ERRSCREEN "e"
#define ARG_ERRFILEAPP "A"
#define ARG_ERRFILEOVER "X"


// MNEMONIC ARGUMENTS
#define ARG_MNEM_LOGPATH "path"
#define ARG_MNEM_LOGFORMAT "format"

#define ARG_MNEM_OUTDISCARD "nout"
#define ARG_MNEM_OUTSCREEN "pout"
#define ARG_MNEM_OUTFILEAPP "appout"
#define ARG_MNEM_OUTFILEOVER "overout"

#define ARG_MNEM_ERRDISCARD "nerr"
#define ARG_MNEM_ERRSCREEN "perr"
#define ARG_MNEM_ERRFILEAPP "apperr"
#define ARG_MNEM_ERRFILEOVER "overerr"

#define ARG_MNEM_HELP "help"
#define ARG_MNEM_VERSION "version"

/**
 * Parse all the command line arguments and build a structure with the parameters set, even default, but the Sub-commands
 * @param argc The maximum number of command line arguments
 * @param argv The array of arguments
 * @return Structure filled with parameters
 */
Command *parseCommand(int argc, char *argv[]);

/**
 * Parse the command starting in <b>str</b> and set two pointers that point the start and the end of the sub-command in <b>str</b>
 *
 * <b>USAGE:</b><br>
 * <code>
 * char p[] = "command";<br>
 * char *start = NULL;<br>
 * char *end = NULL;<br>
 * getNextSubCommand(p, &start, &end);<br>
 * //start[0]="c" end[0]="d"
 * </code>
 *
 * @param str Starting point of the string to parse
 * @param start Return the starting point of the sub-command in <b>str</b>, or <b>NULL</b> when the end of the command is reached
 * @param end Return the ending point (included) of the sub-command in <b>str</b>, or <b>NULL</b> when the end of the command is reached
 */
void getNextSubCommand(char *str, char **start, char **end);

#endif
