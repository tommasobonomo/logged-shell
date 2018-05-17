#include "../statistics/statHelper.h"
#include "../executer/executer.h"
#include "../lib/syscalls.h"
#include "../lib/errors.h"
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include "../parser/parser.h"
#include <sys/types.h>

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

pid_t executeSubCommand(struct SubCommandResult *subcommand, int *pipefds, int pipes, int pipeIndex, bool prevPipe,
                        bool nextPipe)
{
    DEBUG_PRINT("EXECUTING \"%s\"\n", subcommand->subCommand);

    pid_t fid = w_fork();
    if (fid == 0)
    {
        // Child process
        if (prevPipe == true)
        {
            w_dup2(pipefds[(pipeIndex - 1) * 2], STDIN_FILENO);
        }
        if (nextPipe == true)
        {
            w_dup2(pipefds[pipeIndex * 2 + 1], STDOUT_FILENO);
        }

        for (int i = 0; i < (pipes) * 2; i++)
        {
            w_close(pipefds[i]);
        }


        char *args[STRING_LENGHT_MAX];
        int npar = 0;

        char *p = strtok(subcommand->subCommand, " ");
        while (p != NULL)
        {
            args[npar] = p;

            p = strtok(NULL, " ");
            npar++;
        }
        args[npar] = NULL;

        w_execvp(args[0], args);
        //NON REACHABLE CODE
    }
    else
    {
        // Parent process
        int status;
        waitpid(fid, &status, 0);

        if (WEXITSTATUS(status) != 0)
        {
            error_fatal(ERR_CHILD, subcommand->subCommand);
        }

        if (prevPipe)
            close(pipefds[(pipeIndex - 1) * 2]);
        if (nextPipe)
            close(pipefds[pipeIndex * 2 + 1]);

        return fid;
    }
}