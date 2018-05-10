#include "../statistics/statHelper.h"
#include "../executer/executer.h"
#include "../lib/syscalls.h"
#include <stdlib.h>
#include <sys/wait.h>

/**
 * Executes subcommand received from main file in a child process
 * Waits for the process to finish before returning PID
 * @param subcommand the subcommand that will be executed
 * @return the PID of the process executing the subcommand
 */
pid_t executeSubCommand(struct SubCommandResult *subcommand)
{
    if (strcmp(subcommand->subCommand, ";") == 0)
    {
        return CTRL_CMD;
    }
    else
    {
        pid_t fid = frk();
        if (fid == 0)
        {
            // Child process
            DEBUG_PRINT("EXECUTING \"%s\"\n", subcommand->subCommand);
            system(subcommand->subCommand);
            getCurrProcessStats();
            exit(EXIT_SUCCESS);
        }
        else
        {
            // Parent process
            wait(NULL);
            return fid;
        }
    }
}