#include "../statistics/statHelper.h"
#include "../executer/executer.h"
#include "../lib/syscalls.h"
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

bool parentContinue = false;
bool childContinue = false;

void sighandler(int signo)
{
    if (signo == SIGUSR1)
    {
        parentContinue = true;
    }
    else if (signo == SIGUSR2)
    {
        childContinue = true;
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
        wSignal(SIGUSR1, sighandler);
        wSignal(SIGUSR2, sighandler);

        pid_t fid = frk();
        if (fid == 0)
        {
            // Child process

            DEBUG_PRINT("EXECUTING \"%s\"\n", subcommand->subCommand);
            system(subcommand->subCommand);

            kill(getppid(), SIGUSR1);

            while (!childContinue)
            {
                pause();
            }
            exit(EXIT_SUCCESS);
        }
        else
        {
            // Parent process
            while (!parentContinue)
            {
                pause();
            }
            getProcessStats(fid, subcommand);
            kill(fid, SIGUSR2);

            waitpid(fid, NULL, 0);
            return fid;
        }
    }
}