#include "../statistics/statHelper.h"
#include "../executer/executer.h"
#include "../lib/syscalls.h"
#include <sys/wait.h>
#include<signal.h>

pid_t fid;

void child_sighandle(int signo)
{
    if (signo == SIGINT)
    {
        exit(EXIT_SUCCESS);
    }
}

void parent_sighandle(int signo)
{
    if (signo == SIGINT)
    {
        getProcessStats(fid);
        kill(fid, SIGINT);
    }
}

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
        fid = frk();
        if (fid == 0)
        {
            // Child process
            signal(SIGINT, child_sighandle); //TODO wrapper

            DEBUG_PRINT("EXECUTING \"%s\"\n", subcommand->subCommand);
            system(subcommand->subCommand);

            kill(getppid(), SIGINT);

            while (1)
            { sleep(999); } //TODO meke better
        }
        else
        {
            // Parent process
            signal(SIGINT, parent_sighandle); //TODO wrapper

            waitpid(fid, NULL, 0);
            return fid;
        }
    }
}