#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include "../statistics/statHelper.h"
#include "../executer/executer.h"
#include "../lib/syscalls.h"
#include "../lib/errors.h"
#include "../parser/parser.h"
#include "../daemon/daemon.h"

#define READ 0
#define WRITE 1
#define NULLFILE "/dev/null"

int setNullRedirections(struct Command *cmd)
{
    int null_fd = -1;
    if (cmd->output_mode == MODE_DISCARD)
    {
        null_fd = w_open(NULLFILE, O_WRONLY, PERMS);
        dup2(null_fd, STDOUT_FILENO);
    }
    else if (cmd->error_mode == MODE_DISCARD)
    {
        null_fd = w_open(NULLFILE, O_WRONLY, PERMS);
        dup2(null_fd, STDERR_FILENO);
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
        tmpFD = w_open(inFile, O_RDONLY, PERMS);
        dup2(tmpFD, STDIN_FILENO);
    }

    // Redirect output if flag is set
    if (outRedirect)
    {
        tmpFD = w_open(outFile, O_WRONLY | O_CREAT, PERMS);
        dup2(tmpFD, STDOUT_FILENO);
    }
}

//TODO gestore as a thread
void executeSubCommand(SubCommandResult *subCommandResult, int *pipeResult, int *pipefds, int n_pipes,
                       OperatorVars *operatorVars)
{
    DEBUG_PRINT("EXECUTING \"%s\"\n", subCommandResult->subCommand);

    struct timeval start, end;
    double mtime, seconds, useconds;

    pid_t fidGestore = w_fork();
    if (fidGestore == 0)
    {
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
            // Gestore process
            int statusExecuter;
            waitpid(eid, &statusExecuter, 0);

            gettimeofday(&end, NULL);

            seconds = end.tv_sec - start.tv_sec;
            useconds = end.tv_usec - start.tv_usec;
            mtime = seconds + useconds / 1000000;

            getChildrenProcessStats(subCommandResult);
            subCommandResult->pid = eid;
            subCommandResult->totTime = mtime;
            subCommandResult->exitStatus = WEXITSTATUS(statusExecuter);

            //SEND RES TO PARENT
            close(pipeResult[READ]);
            write(pipeResult[WRITE], subCommandResult, sizeof(SubCommandResult));
            close(pipeResult[WRITE]);
        }
        //END GESTORE
    }

    //CHIUSURA PIPES APERTE IN PRECEDENZA
    if (operatorVars->prevPipe)
        w_close(pipefds[(operatorVars->pipeIndex - 1) * 2]);
    if (operatorVars->nextPipe)
        w_close(pipefds[operatorVars->pipeIndex * 2 + 1]);

    if (fidGestore == 0)
    {
        //Gestore
        if (subCommandResult->exitStatus != 0)
        {
            error_fatal(ERR_CHILD, subCommandResult->subCommand);
        }
        else
        {
            exitAndNotifyDaemon(EXIT_SUCCESS);
        }
    }
    else
    {
        //Parent
        if (operatorVars->nextAnd || operatorVars->nextOr)
        {
            int statusGestore;
            int returnGestore;
            waitpid(fidGestore, &statusGestore, 0);

            returnGestore = WEXITSTATUS(statusGestore);
            if (operatorVars->nextAnd)
            {
                if (returnGestore != 0)
                {
                    operatorVars->ignoreNextSubCmd = true;
                    strcpy(operatorVars->ignoreUntil, "&&");
                }
            }
            else if (operatorVars->nextOr)
            {
                if (returnGestore == 0)
                {
                    operatorVars->ignoreNextSubCmd = true;
                    strcpy(operatorVars->ignoreUntil, "||");
                }
            }
        }
    }
}
