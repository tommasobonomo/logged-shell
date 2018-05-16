#include "../statistics/statHelper.h"
#include "../executer/executer.h"
#include "../lib/syscalls.h"
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include "../parser/parser.h"

bool parentContinue = false;
bool childContinue = false;

int countPipes(char *wholeCmd)
{
    char *start = NULL;
    char *end = NULL;

    int pipes = 0;

    //SCANSIONE PER PIPE
    getNextSubCommand(wholeCmd, &start, &end);
    wholeCmd = end + 1;
    while (start != NULL && end != NULL)
    {
        int length = (end - start) * sizeof(*start) + 1;
        if (strncmp(start, "|", (size_t) length) == 0)
            pipes++;

        getNextSubCommand(wholeCmd, &start, &end);
        wholeCmd = end + 1;
    }

    return pipes;
}

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
    wSignal(SIGUSR1, sighandler);
    wSignal(SIGUSR2, sighandler);

    pid_t fid = frk();
    if (fid == 0)
    {
        // Child process

        DEBUG_PRINT("EXECUTING \"%s\"\n", subcommand->subCommand);
        system(subcommand->subCommand); //TODO use execlp

        kill(getppid(), SIGUSR1);

        while (!childContinue)
        {
            pause();
        }
        childContinue = false;
        exit(EXIT_SUCCESS);
    }
    else
    {
        // Parent process
        while (!parentContinue)
        {
            pause();
        }
        parentContinue = false;
        getProcessStats(fid, subcommand);
        kill(fid, SIGUSR2);

        waitpid(fid, NULL, 0);
        return fid;
    }
}