#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "./lib/commands.h"
#include "./parser/parser.h"
#include "./statistics/statHelper.h"
#include "./executer/executer.h"

int main(int argc, char *argv[])
{
    DEBUG_PRINT("  ###########\n");
    DEBUG_PRINT("  ## DEBUG ##\n");
    DEBUG_PRINT("  ###########\n\n");

    struct Command *cmd = parseCommand(argc, argv);

    int pipes = countPipes(cmd->command);
    DEBUG_PRINT("PIPES: %d\n", pipes);

    //CREAZIONE PIPES
    int pipefds[pipes * 2];
    for (int i = 0; i < (pipes) * 2; i += 2)
    {
        pipe(pipefds + i);
    }

    char *p = cmd->command;
    char *start = NULL;
    char *end = NULL;
    bool prevPipe = false;
    bool nextPipe = false;
    int pipeIndex = 0;

    getNextSubCommand(p, &start, &end);
    p = end + 1;

    while (start != NULL && end != NULL)
    {
        struct SubCommandResult *subCmdResult = malloc(sizeof(struct SubCommandResult));

        int length = (end - start) * sizeof(*start) + 1;
        subCmdResult->subCommand = malloc(sizeof(char) * (length + 1));
        sprintf(subCmdResult->subCommand, "%.*s", length, start);

        //READ OPERATOR
        getNextSubCommand(p, &start, &end);
        p = end + 1;

        if (start != NULL && end != NULL)
        {
            int lengthOperator = (end - start) * sizeof(*start) + 1;
            if (strncmp(start, "|", (size_t) lengthOperator) == 0)
            {
                nextPipe = true;
                //TODO redir output su fd corrente
            }
            else if (strncmp(start, ";", (size_t) lengthOperator) == 0)
            {
                //fare niente
            }
        }//else there is no operator
        //END - READ OPERATOR

        subCmdResult->pid = executeSubCommand(subCmdResult, pipefds, pipes, pipeIndex, prevPipe, nextPipe);

        getChildrenProcessStats();

        //SAVING CURRENT SUBCOMMAND
        DEBUG_PRINT("PID of process that executed command: %d\n", subCmdResult->pid);
        cmd->subCommandResults[cmd->n_subCommands] = subCmdResult;
        cmd->n_subCommands++;

        if (start != NULL && end != NULL)
        {
            getNextSubCommand(p, &start, &end);
            p = end + 1;
        }

        if (nextPipe == true)
            pipeIndex++;
        prevPipe = nextPipe;
        nextPipe = false;
    }




    // FREEING DYNAMICALLY ALLOCATED MEMORY
    for (int i = 0; i < cmd->n_subCommands; i++)
    {
        free(cmd->subCommandResults[i]->subCommand);
        free(cmd->subCommandResults[i]);
    }
    free(cmd);
    // END FREEING DYNAMICALLY ALLOCATED MEMORY

    return 0;
}