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

pid_t executeSubCommand(struct SubCommandResult *subcommand, int *pipefds, int pipes, int pipeIndex, bool prevPipe,
                        bool nextPipe)
{
    wSignal(SIGUSR1, sighandler);
    wSignal(SIGUSR2, sighandler);

    DEBUG_PRINT("EXECUTING \"%s\"\n", subcommand->subCommand);

    pid_t fid = frk();
    if (fid == 0)
    {
        // Child process
        if (prevPipe == true)
        {
            DEBUG_PRINT("sono %s LEGGO DA id:%d\n", subcommand->subCommand, (pipeIndex - 1) * 2);
            //TODO wrapper
            if (dup2(pipefds[(pipeIndex - 1) * 2], STDIN_FILENO) < 0)
            {
                perror("a");
                exit(EXIT_FAILURE);
            }
            DEBUG_PRINT("sono %s LEGGO DA addr:%d\n", subcommand->subCommand, pipefds[(pipeIndex - 1) * 2]);
        }
        if (nextPipe == true)
        {
            DEBUG_PRINT("sono %s SCRIVO SU id:%d\n", subcommand->subCommand, pipeIndex * 2 + 1);
            //TODO wrapper
            if (dup2(pipefds[pipeIndex * 2 + 1], STDOUT_FILENO) < 0)
            {
                perror("b");
                exit(EXIT_FAILURE);
            }
            DEBUG_PRINT("sono %s SCRIVO SU addr:%d\n", subcommand->subCommand, pipefds[pipeIndex * 2 + 1]);

        }

        for (int i = 0; i < (pipes) * 2; i++)
        {
            DEBUG_PRINT("sono %s CHIUDO addr:%d\n", subcommand->subCommand, pipefds[i]);
            close(pipefds[i]);
        }


        char *args[20]; //TODO DIO
        int npar = 0;

        char *p = strtok(subcommand->subCommand, " ");
        while (p != NULL)
        {
            args[npar] = p;

            p = strtok(NULL, " ");
            npar++;
        }
        args[npar] = NULL;

        if (execvp(args[0], args) < 0) //TODO WRAPPER
        {
            printf("*** ERROR: exec failed\n");
            exit(1);
        }
        //NON REACHABLE CODE
    }
    else
    {
        // Parent process
        waitpid(fid, NULL, 0);
        if (prevPipe)
            close(pipefds[(pipeIndex - 1) * 2]);
        if (nextPipe)
            close(pipefds[pipeIndex * 2 + 1]);

        return fid;
    }
}