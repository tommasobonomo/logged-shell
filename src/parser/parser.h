#ifndef PARSER_H
#define PARSER_H

/**
 * Parse all the command line arguments and build a structure with the parameters set, even default, but the Sub-commands
 * @param argc The maximum number of command line arguments
 * @param argv The array of arguments
 * @return Structure filled with parameters
 */
struct Command *parseCommand(int argc, char *argv[]);

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