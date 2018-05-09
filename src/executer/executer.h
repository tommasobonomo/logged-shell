#ifndef EXECUTER_H
#define EXECUTER_H

#include <sys/types.h>

/**
 * Executes subcommand received from main file
 * @param subcommand the subcommand that will be executed
 */
pid_t executeSubCommand(char subcommand[]);

#endif