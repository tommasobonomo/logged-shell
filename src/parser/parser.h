#ifndef PARSER_H
#define PARSER_H

#include "../lib/commands.h"

// NORMAL ARGUMENTS
//TODO normal args

// MNEMONIC ARGUMENTS
#define ARG_MEM_LOGPATH "path"
#define ARG_MEM_LOGFORMAT "format"

#define ARG_MEM_OUTDISCARD "nout"
#define ARG_MEM_OUTSCEEN "pout"
#define ARG_MEM_OUTLOG "outtolog"
#define ARG_MEM_OUTFILEAPP "fileappout"
#define ARG_MEM_OUTFILEOVER "fileoverout"

#define ARG_MEM_ERRDISCARD "nerr"
#define ARG_MEM_ERRSCEEN "perr"
#define ARG_MEM_ERRLOG "errtolog"
#define ARG_MEM_ERRFILEAPP "fileapperr"
#define ARG_MEM_ERRFILEOVER "fileovererr"

#define ARG_MEM_DONTCREATELOG "nlog"

#define ARG_MEM_HELP "help"
#define ARG_MEM_VERSION "version"

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
