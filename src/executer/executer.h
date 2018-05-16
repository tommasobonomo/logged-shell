#ifndef EXECUTER_H
#define EXECUTER_H

#include "../lib/commands.h"
#include <sys/types.h>

#define CTRL_CMD (-1)

/**
 * Executes subcommand received from main file in a forked process
 * The parent process waits for it's child to finish execution before returning the PID
 * @param subcommand the subcommand that will be executed, as a struct SubCommandResult defined in src/lib/commands.h
 * @return the PID of the process that executed the subcommand
 */
pid_t executeSubCommand(struct SubCommandResult *subcommand, int *pipefds, int pipes, int pipeIndex, bool prevPipe,
                        bool nextPipe);

int countPipes(char *wholeCmd);

#endif