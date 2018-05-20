#include "../statistics/statHelper.h"
#include "../executer/executer.h"
#include "../lib/syscalls.h"
#include "../lib/errors.h"
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include "../parser/parser.h"
#include "../daemon/daemon.h"
#include <sys/types.h>
#include <sys/time.h>

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
        if (strncmp(start, "|", (size_t)length) == 0)
            pipes++;

        getNextSubCommand(wholeCmd, &start, &end);
        wholeCmd = end + 1;
    }

    return pipes;
}

void vectorizeStringArguments(char argsString[], char *argsVect[])
{
    int npar = 0;

    char *p = strtok(argsString, " ");
    while (p != NULL)
    {
        argsVect[npar] = p;

        p = strtok(NULL, " ");
        npar++;
    }
    argsVect[npar] = NULL;
}

void managePipes(int *pipefds, int pipes, int pipeIndex, bool prevPipe, bool nextPipe)
{
    if (prevPipe == true)
    {
        w_dup2(pipefds[(pipeIndex - 1) * 2], STDIN_FILENO);
    }
    if (nextPipe == true)
    {
        w_dup2(pipefds[pipeIndex * 2 + 1], STDOUT_FILENO);
    }

    int i;
    for (i = pipeIndex * 2; i < (pipes)*2; i++)
    {
        w_close(pipefds[i]);
    }
}

void executeSubCommand(SubCommandResult *subCommandResult, int msqid, int *pipefds, int pipes, int pipeIndex,
                       bool prevPipe,
                       bool nextPipe, bool nextAnd, bool nextOr)
{
    DEBUG_PRINT("EXECUTING \"%s\"\n", subCommandResult->subCommand);

    int returnExecuter;

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

            //PREPARE PIPES IF NEEDED
            managePipes(pipefds, pipes, pipeIndex, prevPipe, nextPipe);

            //PREPARE ARGS
            char *args[MAX_ARGUMENTS];
            vectorizeStringArguments(subCommandResult->subCommand, args);

            //EXECUTE SUBCOMMAND
            w_execvp(args[0], args);

            //NON REACHABLE CODE
            exit(1);
        }
        else
        {
            // Gestore process
            int statusExecuter;
            waitpid(fid, &statusExecuter, 0);

            gettimeofday(&end, NULL);

            seconds = end.tv_sec - start.tv_sec;
            useconds = end.tv_usec - start.tv_usec;
            mtime = seconds + useconds / 1000000;

            getChildrenProcessStats(subCommandResult);
            subCommandResult->pid = fid;
            subCommandResult->totTime = mtime;

            send_msg(msqid, subCommandResult);

            returnExecuter = WEXITSTATUS(statusExecuter);
            if (returnExecuter != 0)
            {
                error_fatal(ERR_CHILD, subCommandResult->subCommand);
            }
        }
        //END GESTORE
    }

    if (prevPipe)
        w_close(pipefds[(pipeIndex - 1) * 2]);
    if (nextPipe)
        w_close(pipefds[pipeIndex * 2 + 1]);

    if (fidGestore == 0)
    {
        //Gestore
        exit(returnExecuter);
    }
    else
    {
        //Parent
        if (nextAnd || nextOr)
        {
            int statusGestore;
            int returnGestore;
            waitpid(fidGestore, &statusGestore, 0);

            returnGestore = WEXITSTATUS(statusGestore);
            if (nextAnd)
            {
                if (returnGestore != 0)
                {
                    exit(returnGestore);
                }
            }
            else if (nextOr)
            {
                if (returnGestore == 0)
                {
                    exit(returnGestore);
                }
            }
        }
    }
}
