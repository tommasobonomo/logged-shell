#ifndef EXECUTER_H
#define EXECUTER_H

#include "../lib/commands.h"
#include <sys/types.h>
#include <string.h>

typedef struct OperatorVars
{
    int pipeIndex;
    bool prevPipe;
    bool nextPipe;
    bool nextAnd;
    bool nextOr;
    bool ignoreNextSubCmd;
    char ignoreUntil[2 + 1];

    bool inRedirect;
    bool outRedirect;
    char inFile[MAX_STRING_LENGHT];
    char outFile[MAX_STRING_LENGHT];
} OperatorVars;

void executeSubCommand(SubCommandResult *subCommandResult, int *pipefds, int n_pipes, pthread_t *threads,
                       OperatorVars *operatorVars);

int countPipes(char *wholeCmd);

int manageQuietMode(struct Command *cmd);

void manageRedirections(bool inRedirect, bool outRedirect, char *inFile, char *outFile);

#endif
