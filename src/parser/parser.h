#ifndef PARSER_H
#define PARSER_H

/**
 * Parse all the command line arguments and build a structure with the parameters set, even default, but the Sub-commands
 * @param argc The maximum number of command line arguments
 * @param argv The array of arguments
 * @return Structure filled with parameters
 */
struct Command *parseCommand(int argc, char **argv);

#endif