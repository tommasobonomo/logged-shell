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

void executeSubCommand(struct SubCommandResult *subCommandResult, int *pipefds, int pipes, int pipeIndex, bool prevPipe,
                       bool nextPipe)
{
    DEBUG_PRINT("\nEXECUTING \"%s\"\n", subCommandResult->subCommand);

    struct timeval start, end;
    double mtime, seconds, useconds;

    gettimeofday(&start, NULL);

    pid_t fidGestore = w_fork();
    if (fidGestore == 0)
    {
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

            int i;
            for (i = pipeIndex * 2; i < (pipes) * 2; i++)
            {
                w_close(pipefds[i]);
            }

            char *args[STRING_LENGHT_MAX];
            int npar = 0;

            char *p = strtok(subCommandResult->subCommand, " ");
            while (p != NULL)
            {
                args[npar] = p;

                p = strtok(NULL, " ");
                npar++;
            }
            args[npar] = NULL;

            w_execvp(args[0], args);
            //NON REACHABLE CODE
            exit(1);
        }
        else
        {
            // Parent process
            int status;
            waitpid(fid, &status, 0);

            gettimeofday(&end, NULL);

            if (WEXITSTATUS(status) != 0)
            {
                error_fatal(ERR_CHILD, subCommandResult->subCommand);
            }

            seconds = end.tv_sec - start.tv_sec;
            useconds = end.tv_usec - start.tv_usec;
            mtime = seconds + (double) useconds / 1000000;

            //getChildrenProcessStats();
            subCommandResult->pid = fid;
            DEBUG_PRINT("Elapsed time: %f seconds \n", mtime);
            DEBUG_PRINT("PID of process that executed command: %d\n", subCommandResult->pid);
        }
    }

    if (prevPipe)
        w_close(pipefds[(pipeIndex - 1) * 2]);
    if (nextPipe)
        w_close(pipefds[pipeIndex * 2 + 1]);

    if (fidGestore == 0)
    {
        exit(0);
    }
}

