#ifndef EXECUTER_H
#define EXECUTER_H

#include "../lib/commands.h"
#include <sys/types.h>
#include <string.h>

void executeSubCommand(SubCommandResult *subCommandResult, int *pipeResult, int *pipefds, int pipes, int pipeIndex,
                       bool prevPipe, bool nextPipe, bool nextAnd, bool nextOr);

int countPipes(char *wholeCmd);

int setNullRedirections(struct Command *cmd);

void manageRedirections(char *redirectFile, bool inRedirect, bool outRedirect, int *inFD, int *outFD);

void resetRedirections(bool inRedirect, bool outRedirect, int *inFD, int *outFD);

#endif
