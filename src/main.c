#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include "./lib/commands.h"
#include "./parser/parser.h"
#include "./executer/executer.h"

int main(int argc, char *argv[])
{
    DEBUG_PRINT("  ###########\n");
    DEBUG_PRINT("  ## DEBUG ##\n");
    DEBUG_PRINT("  ###########\n\n");

    struct Command *cmd = parseCommand(argc, argv);

    char *p = cmd->command;
    char *start = NULL;
    char *end = NULL;

    do
    {
        getNextSubCommand(p, &start, &end);
        p = end + 1;
        if (start != NULL && end != NULL)
        {
            struct SubCommandResult *subCmdResult = malloc(sizeof(struct SubCommandResult));

            int length = (end - start) * sizeof(*start) + 1;
            subCmdResult->subCommand = malloc(sizeof(char) * (length + 1));
            sprintf(subCmdResult->subCommand, "%.*s", length, start);

            subCmdResult->pid = executeSubCommand(subCmdResult);

            if (subCmdResult->pid != CTRL_CMD)
            {
                //SAVING CURRENT SUBCOMMAND
                DEBUG_PRINT("PID of process that executed command: %d\n", subCmdResult->pid);
                cmd->subCommandResults[cmd->n_subCommands] = subCmdResult;
                cmd->n_subCommands++;
            }
            else
            {
                free(subCmdResult);
            }
        }

    } while (start != NULL && end != NULL);


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