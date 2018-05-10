#include "../lib/syscalls.h"
#include "../lib/commands.h"
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
    pid_t fid = frk();
    if (fid == 0)
    { // Child process
        DEBUG_PRINT("EXECUTING \"%s\"\n", subcommand->subCommand);
        system(subcommand->subCommand);
        exit(EXIT_SUCCESS);
    }
    else
    {
        // Parent process
        wait(NULL);
        return fid;
    }
}