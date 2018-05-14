#include "../statistics/statHelper.h"
#include "../executer/executer.h"
#include "../lib/syscalls.h"
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

pid_t fid;

void sighandler(int signo)
{
    if (signo == SIGUSR1)
    {
        getProcessStats(fid); // TODO passare anche struttura subcommand
        kill(fid, SIGUSR2);
    }
    else if (signo == SIGUSR2)
    {
        exit(EXIT_SUCCESS);
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
        signal(SIGUSR1, sighandler); //TODO wrapper
        signal(SIGUSR2, sighandler); //TODO wrapper

        fid = frk();
        if (fid == 0)
        {
            // Child process

            DEBUG_PRINT("EXECUTING \"%s\"\n", subcommand->subCommand);
            system(subcommand->subCommand);

            kill(getppid(), SIGUSR1);

            while (1)
            { sleep(999); } //TODO meke better
        }
        else
        {
            // Parent process

            waitpid(fid, NULL, 0);
            return fid;
        }
    }
}