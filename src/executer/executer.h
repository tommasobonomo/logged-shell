#ifndef EXECUTER_H
#define EXECUTER_H

#include <sys/types.h>

/**
 * Executes subcommand received from main file in a forked process
 * The parent process waits for it's child to finish execution before returning the PID
 * @param subcommand the subcommand that will be executed
 * @return the PID of the process that executed the subcommand
 */
pid_t executeSubCommand(char subcommand[]);

#endif