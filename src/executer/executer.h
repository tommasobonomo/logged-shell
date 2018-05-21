#ifndef EXECUTER_H
#define EXECUTER_H

#include "../lib/commands.h"
#include <sys/types.h>
#include <string.h>

void executeSubCommand(SubCommandResult *subCommandResult, int *pipeResult, int *pipefds, int pipes, int pipeIndex,
                       bool prevPipe, bool nextPipe, bool nextAnd, bool nextOr);

int countPipes(char *wholeCmd);

#endif
