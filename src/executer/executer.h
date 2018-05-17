#ifndef EXECUTER_H
#define EXECUTER_H

#include "../lib/commands.h"
#include <sys/types.h>

void executeSubCommand(struct SubCommandResult *subCommandResult, int *pipefds, int pipes, int pipeIndex, bool prevPipe,
                       bool nextPipe);

int countPipes(char *wholeCmd);

#endif