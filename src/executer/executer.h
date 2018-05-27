#ifndef EXECUTER_H
#define EXECUTER_H

#include "../lib/commands.h"
#include <sys/types.h>
#include <string.h>

typedef struct FlagRedirectVars
{
    int output_mode;
    char output_path[MAX_STRING_LENGTH];
    int error_mode;
    char error_path[MAX_STRING_LENGTH];
} FlagRedirectVars;

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
    char inFile[MAX_STRING_LENGTH];
    char outFile[MAX_STRING_LENGTH];
    int outMode;

    char currentDirectory[MAX_STRING_LENGTH-3];
} OperatorVars;

void executeSubCommand(SubCommandResult *subCommandResult, int *pipefds, int n_pipes, pthread_t *threads,
                       OperatorVars *operatorVars, FlagRedirectVars *flagVars);

/**
 * Count the number od the pipes "|" to expect in the whole command
 * @param ptWholeCmd Pointer to the string containing the whole command (es. "ls -a | wc")
 * @return The numer of the pipes in the whole command
 */
int countPipes(char *ptWholeCmd);

int manageQuietMode(struct Command *cmd);

void manageInterCommandRedirections(bool inRedirect, bool outRedirect, char *inFile, char *outFile, int outMode);

#endif
