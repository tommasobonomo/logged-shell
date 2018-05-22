#ifndef EXECUTER_H
#define EXECUTER_H

#include "../lib/commands.h"
#include <sys/types.h>
#include <string.h>

typedef struct ServiceVars
{
    int pipeIndex;
    bool prevPipe;
    bool nextPipe;
    bool nextAnd;
    bool nextOr;
    bool ignoreNextSubCmd;
    char ignoreUntil[2 + 1];
} ServiceVars;

void executeSubCommand(SubCommandResult *subCommandResult, int *pipeResult, int *pipefds, int n_pipes,
                       struct ServiceVars *serviceVars);

int countPipes(char *wholeCmd);

#endif
