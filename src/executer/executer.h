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

typedef struct RedirectVars
{
    bool inRedirect;
    bool outRedirect;
    char inFile[MAX_STRING_LENGHT];
    char outFile[MAX_STRING_LENGHT];
} RedirectVars;

void executeSubCommand(SubCommandResult *subCommandResult, int *pipeResult, int *pipefds, int n_pipes,
                       ServiceVars *serviceVars, RedirectVars *redirectVars);

int countPipes(char *wholeCmd);

int setNullRedirections(struct Command *cmd);

void manageRedirections(bool inRedirect, bool outRedirect, char *inFile, char *outFile);

void resetRedirections(bool inRedirect, bool outRedirect, int *inFD, int *outFD);

#endif
