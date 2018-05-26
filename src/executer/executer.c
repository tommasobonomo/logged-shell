#include "./executer.h"
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <pthread.h>
#include "../statistics/statHelper.h"
#include "../lib/syscalls.h"
#include "../lib/errors.h"
#include "../parser/parser.h"
#include "../daemon/daemon.h"

#define NULLFILE "/dev/null"

typedef struct ThreadArgs
{
    int ID;
    pthread_t *threads;
    SubCommandResult *subCommandResult;
    pid_t eid;
    OperatorVars *operatorVars;
    struct timeval start;
} ThreadArgs;

int setNullRedirections(struct Command *cmd)
{
    int null_fd = -1;
    if (cmd->output_mode == MODE_DISCARD || cmd->error_mode == MODE_DISCARD)
    {
        null_fd = w_open(NULLFILE, O_WRONLY, USER_PERMS);
        if (cmd->output_mode == MODE_DISCARD)
        {
            dup2(null_fd, STDOUT_FILENO);
        }
        if (cmd->error_mode == MODE_DISCARD)
        {
            dup2(null_fd, STDERR_FILENO);
        }
    }
    return null_fd;
}

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

void managePipes(int *pipefds, int n_pipes, int pipeIndex, bool prevPipe, bool nextPipe)
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
    for (i = pipeIndex * 2; i < (n_pipes) * 2; i++)
    {
        w_close(pipefds[i]);
    }
}

void manageRedirections(bool inRedirect, bool outRedirect, char *inFile, char *outFile)
{
    int tmpFD;

    // Redirect input if flag is set
    if (inRedirect)
    {
        tmpFD = w_open(inFile, O_RDONLY, USER_PERMS);
        dup2(tmpFD, STDIN_FILENO);
    }

    // Redirect output if flag is set
    if (outRedirect)
    {
        tmpFD = w_open(outFile, O_WRONLY | O_CREAT, USER_PERMS);
        dup2(tmpFD, STDOUT_FILENO);
    }
}

void finalizeSubCommand(ThreadArgs *args)
{
    struct rusage childUsage;
    int statusExecuter;
    struct timeval end;
    double mtime, seconds, useconds;

    wait4(args->eid, &statusExecuter, 0, &childUsage);

    gettimeofday(&end, NULL);

    seconds = end.tv_sec - args->start.tv_sec;
    useconds = end.tv_usec - args->start.tv_usec;
    mtime = seconds + useconds / 1000000;

    saveProcessStats(args->subCommandResult, &childUsage);
    args->subCommandResult->pid = args->eid;
    args->subCommandResult->totRealTime = mtime;
    args->subCommandResult->exitStatus = WEXITSTATUS(statusExecuter);

    if (args->subCommandResult->exitStatus != 0)
    {
        //TODO log a video subCommandResult->subCommand
    }


    if (!args->operatorVars->nextPipe)
    {
        if (args->operatorVars->nextAnd)
        {
            if (args->subCommandResult->exitStatus != 0)
            {
                args->operatorVars->ignoreNextSubCmd = true;
                strcpy(args->operatorVars->ignoreUntil, "&&");
            }
        }
        else if (args->operatorVars->nextOr)
        {
            if (args->subCommandResult->exitStatus == 0)
            {
                args->operatorVars->ignoreNextSubCmd = true;
                strcpy(args->operatorVars->ignoreUntil, "||");
            }
        }
    }
}

void *waitExecuterAndFinalizeSubCommand(void *argument)
{
    ThreadArgs *threadArgs = (ThreadArgs *) argument;
    finalizeSubCommand(argument);
    if (threadArgs->operatorVars->prevPipe)
    {
        pthread_join(threadArgs->threads[threadArgs->ID - 1], NULL);
    }
    free(threadArgs->operatorVars);
    free(argument);
    pthread_exit(NULL);
}

void executeSubCommand(SubCommandResult *subCommandResult, int *pipefds, int n_pipes, pthread_t *threads,
                       OperatorVars *operatorVars)
{
    DEBUG_PRINT("EXECUTING \"%s\"\n", subCommandResult->subCommand);

    struct timeval start;

    gettimeofday(&start, NULL);

    pid_t eid = w_fork();
    if (eid == 0)
    {
        // Executer process

        //PREPARE PIPES IF NEEDED
        managePipes(pipefds, n_pipes, operatorVars->pipeIndex, operatorVars->prevPipe, operatorVars->nextPipe);

        //PREPARE REDIRECTIONS IF NEEDED
        manageRedirections(operatorVars->inRedirect, operatorVars->outRedirect, operatorVars->inFile,
                           operatorVars->outFile);

        //PREPARE ARGS
        char *args[MAX_ARGUMENTS];
        vectorizeStringArguments(subCommandResult->subCommand, args);

        //EXECUTE SUBCOMMAND
        w_execvp(args[0], args); //TODO gestire un comando nella cartella corrente e non solo nella path di sistema

        //UNREACHABLE CODE
    }
    else
    {
        //Parent

        //CHIUSURA PIPES APERTE IN PRECEDENZA
        if (operatorVars->prevPipe)
            w_close(pipefds[(operatorVars->pipeIndex - 1) * 2]);
        if (operatorVars->nextPipe)
            w_close(pipefds[operatorVars->pipeIndex * 2 + 1]);

        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->ID = operatorVars->pipeIndex;
        args->threads = threads;
        args->subCommandResult = subCommandResult;
        args->eid = eid;
        args->operatorVars = operatorVars;
        args->start = start;
        args->subCommandResult->pgid = getpgid(eid);
        args->subCommandResult->sid = getsid(eid);



        if (operatorVars->nextPipe)
        {
            args->operatorVars = malloc(sizeof(OperatorVars));
            *args->operatorVars = *operatorVars;
            pthread_create(&threads[operatorVars->pipeIndex], NULL, waitExecuterAndFinalizeSubCommand, args);
        }
        else
        {
            if (operatorVars->prevPipe)
            {
                DEBUG_PRINT("Aspetto thread %d\n", operatorVars->pipeIndex - 1);
                pthread_join(threads[operatorVars->pipeIndex - 1], NULL);
            }
            finalizeSubCommand(args);
            free(args);
        }
    }
}
