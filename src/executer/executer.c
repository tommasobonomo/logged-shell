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

int countPipes(char *ptWholeCmd)
{
    char *start = NULL;
    char *end = NULL;

    int pipes = 0;

    //SCAN FOR PIPE
    getNextSubCommand(ptWholeCmd, &start, &end);
    ptWholeCmd = end + 1;
    while (start != NULL && end != NULL)
    {
        int length = (end - start + 1) * sizeof(char);
        if (strncmp(start, "|", (size_t) length) == 0)
            pipes++;

        getNextSubCommand(ptWholeCmd, &start, &end);
        ptWholeCmd = end + 1;
    }

    return pipes;
}

/**
 * Fill an array with a tokenized version of a string containing a program and its parameters
 * @param argsString String of a program and its parameters
 * @param argsVect Array to fill
 */
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

/**
 * Handy function responsabile for opening pipes, when necessary, and connect processes to the right sides of it<br>
 * This function will also close unused pipes
 * @param pipefds Array of pipe file descriptors
 * @param n_pipes Number of pipes in <b>pipefds</b><br>
 *                This number is half of the elements in <b>pipefds</b>: 1 pipe --> pipefds[2]
 * @param pipeIndex Number of pipes ("|") already managed
 * @param prevPipe Whether the previous operator was a pipe
 * @param nextPipe Whether the next operator is a pipe
 */
void openAndManagePipesIfNeeded(int *pipefds, int n_pipes, int pipeIndex, bool prevPipe, bool nextPipe)
{
    if (prevPipe == true)
    {
        //Open read side
        w_dup2(pipefds[(pipeIndex - 1) * 2], STDIN_FILENO);
    }
    if (nextPipe == true)
    {
        //Open write side
        w_dup2(pipefds[pipeIndex * 2 + 1], STDOUT_FILENO);
    }

    int i;
    for (i = pipeIndex * 2; i < n_pipes * 2; i++)
    {
        //Close unused next pipe sides
        w_close(pipefds[i]);
    }
}

/**
 * Handy function responsabile for close previous opened pipes, if any
 * @param pipefds Array of pipe file descriptors
 * @param pipeIndex Number of pipes ("|") already managed
 * @param prevPipe Whether the previous operator was a pipe
 * @param nextPipe Whether the next operator is a pipe
 */
void closeOpenedPipes(int *pipefds, int pipeIndex, bool prevPipe, bool nextPipe)
{
    if (prevPipe)
    {
        //Close read side
        w_close(pipefds[(pipeIndex - 1) * 2]);
    }
    if (nextPipe)
    {
        //Close write side
        w_close(pipefds[pipeIndex * 2 + 1]);
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

/**
 * Wait for previous command to finish, save statistics and prepare environment for the next command to come
 * @param args Internal variables
 */
void finalizeSubCommand(ThreadArgs *args)
{
    struct rusage childUsage;
    int statusExecuter;
    struct timeval end;
    double mtime, seconds, useconds;

    wait4(args->eid, &statusExecuter, 0, &childUsage);
    if (strncmp(args->subCommandResult->subCommand, "cd ", 3) == 0)
    {
        char *currentDir = getcwd(NULL, 0);
        char *selectedPath = args->subCommandResult->subCommand + 3;
        DEBUG_PRINT("Working directory:  %s\n", currentDir);
        DEBUG_PRINT("operatorVars     :  _%s_\n", selectedPath);

        if (strcmp(currentDir, selectedPath) != 0)
        {
            statusExecuter = w_chdir(selectedPath); //TODO check if exit status are corrects
            DEBUG_PRINT("Nuovo path: %s\n", getcwd(NULL, 0));
        }
    }


    gettimeofday(&end, NULL);

    seconds = end.tv_sec - args->start.tv_sec;
    useconds = end.tv_usec - args->start.tv_usec;
    mtime = seconds + useconds / 1000000;

    saveProcessStats(args->subCommandResult, &childUsage);
    args->subCommandResult->pid = args->eid;
    args->subCommandResult->totRealTime = mtime;
    args->subCommandResult->exitStatus = WEXITSTATUS(statusExecuter);

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

/**
 * A version of <b>finalizeSubCommand(...)</b> made for threads<br>
 * This function provide the business logic to manage the thread that finalized a subCommand within a pipe
 * @param args Internal variables (<b>ThreadArgs</b>)
 * @return void
 */
void *finalizePipedSubCommand(void *args)
{
    finalizeSubCommand(args);
    ThreadArgs *threadArgs = (ThreadArgs *) args;
    if (threadArgs->operatorVars->prevPipe)
    {
        pthread_join(threadArgs->threads[threadArgs->ID - 1], NULL);
    }
    free(threadArgs->operatorVars);
    free(args);
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
        openAndManagePipesIfNeeded(pipefds, n_pipes, operatorVars->pipeIndex, operatorVars->prevPipe,
                                   operatorVars->nextPipe);

        //PREPARE REDIRECTIONS IF NEEDED
        manageRedirections(operatorVars->inRedirect, operatorVars->outRedirect, operatorVars->inFile,
                           operatorVars->outFile);

        //PREPARE ARGS
        char *args[MAX_ARGUMENTS];
        vectorizeStringArguments(subCommandResult->subCommand, args);

        //EXECUTE SUBCOMMAND
        if (strcmp(args[0], "cd") == 0) //builtin cd
        {
            strcpy(operatorVars->currentDirectory, args[1]);
            DEBUG_PRINT("Copied path: %s\n", operatorVars->currentDirectory);
            exit(EXIT_SUCCESS);
        }
        else //others
        {
            w_execvp(args[0], args); //TODO gestire un comando nella cartella corrente e non solo nella path di sistema
        }
        //UNREACHABLE CODE
    }
    else
    {
        //Parent

        closeOpenedPipes(pipefds, operatorVars->pipeIndex, operatorVars->prevPipe, operatorVars->nextPipe);

        ThreadArgs *args =
                malloc(sizeof(ThreadArgs)); //FREED IN finalizePipedSubCommand(...) (by thread) OR at the and of this scope (by parent)
        args->ID = operatorVars->pipeIndex;
        args->threads = threads;
        args->subCommandResult = subCommandResult;
        args->eid = eid;
        args->operatorVars = operatorVars;
        args->start = start;

        if (operatorVars->nextPipe)
        {
            args->operatorVars = malloc(sizeof(OperatorVars)); //FREED IN finalizePipedSubCommand(...)
            *args->operatorVars = *operatorVars;
            pthread_create(&threads[operatorVars->pipeIndex], NULL, finalizePipedSubCommand, args);
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
