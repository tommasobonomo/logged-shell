#ifndef EXECUTER_H
#define EXECUTER_H

#include "../lib/commands.h"
#include <sys/types.h>
#include <string.h>

void executeSubCommand(SubCommandResult *subCommandResult, int *pipeResult, int *pipefds, int pipes, int pipeIndex, bool prevPipe, bool nextPipe, bool nextAnd, bool nextOr, bool inRedirect, bool outRedirect, char *inFile, char *outFile);

int countPipes(char *wholeCmd);

int setNullRedirections(struct Command *cmd);

void manageRedirections(bool inRedirect, bool outRedirect, char *inFile, char *outFile);

void resetRedirections(bool inRedirect, bool outRedirect, int *inFD, int *outFD);

#endif
