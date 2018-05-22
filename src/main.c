#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "./lib/syscalls.h"
#include "./lib/commands.h"
#include "./parser/parser.h"
#include "./statistics/statHelper.h"
#include "./executer/executer.h"
#include "./daemon/daemon.h"

#define READ 0
#define WRITE 1

int msqid;
pid_t pid_main;

void initServiceVars(ServiceVars *serviceVars)
{
    serviceVars->pipeIndex = 0;
    serviceVars->prevPipe = false;
    serviceVars->nextPipe = false;
    serviceVars->nextAnd = false;
    serviceVars->nextOr = false;
    serviceVars->ignoreNextSubCmd = false;
    serviceVars->ignoreUntil[0] = '\0';
}

void serviceVarsNext(ServiceVars *serviceVars)
{
    if (serviceVars->nextPipe == true)
    {
        serviceVars->pipeIndex++;
    }
    serviceVars->prevPipe = serviceVars->nextPipe;
    serviceVars->nextPipe = false;

    serviceVars->nextAnd = false;
    serviceVars->nextOr = false;
}

// Handler di segnali per mandare un segnale di chiusura al demone comunque
void interrupt_sighandler(int signum)
{
    switch (signum)
    {
        case SIGINT:
        case SIGTERM:
        case SIGQUIT:
            exitAndNotifyDaemon(EXIT_SUCCESS);
            break;
        default:
            DEBUG_PRINT("Signal: %d\n", signum);
            exitAndNotifyDaemon(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    pid_main = getpid();
    msqid = check(); //Comunicazione iniziale con demone, va fatta all'inizio dell'esecuzione

    int i;
    for (i = 1; i <= 64; i++)
    {
        if (i != SIGCONT && i != SIGCHLD)
        {
            signal(i, interrupt_sighandler);
        }
    }

    //SANITY CHECKS
    sanityCheck();

    Command *cmd = parseCommand(argc, argv);

    //CREAZIONE PIPES |
    int n_pipes = countPipes(cmd->command);
    int *pipefds = malloc(2 * n_pipes * sizeof(int));
    for (i = 0; i < n_pipes * 2; i += 2)
    {
        w_pipe(pipefds + i);
    }

    //CREAZIONE PIPE padre-figlio
    int pipeResult[2];
    w_pipe(pipeResult);

    //ESECUZIONE SUBCOMANDI
    char *p = cmd->command;
    char *start = NULL;
    char *end = NULL;
    int lengthOperator;
    ServiceVars serviceVars;
    initServiceVars(&serviceVars);

    getNextSubCommand(p, &start, &end);
    p = end + 1;

    while (start != NULL && end != NULL)
    {
        SubCommandResult *tmpSubCmdResult = malloc(sizeof(SubCommandResult));

        int length = (end - start) * sizeof(*start) + 1;
        sprintf(tmpSubCmdResult->subCommand, "%.*s", length, start);

        //READ OPERATOR
        getNextSubCommand(p, &start, &end);
        p = end + 1;

        if (start != NULL && end != NULL)
        {
            lengthOperator = (end - start) * sizeof(*start) + 1;

            if (strncmp(start, "|", (size_t) lengthOperator) == 0)
            {
                serviceVars.nextPipe = true;
            }
            else if (strncmp(start, "&&", (size_t) lengthOperator) == 0)
            {
                serviceVars.nextAnd = true;
            }
            else if (strncmp(start, "||", (size_t) lengthOperator) == 0)
            {
                serviceVars.nextOr = true;
            }
            else if (strncmp(start, ";", (size_t) lengthOperator) == 0)
            {
                //fare niente
            }
        } //else there is no operator
        //END - READ OPERATOR

        tmpSubCmdResult->ID = cmd->n_subCommands++;
        if (!serviceVars.ignoreNextSubCmd)
        {
            executeSubCommand(tmpSubCmdResult, pipeResult, pipefds, n_pipes, &serviceVars);
        }
        else
        {
            cmd->subCommandResults[tmpSubCmdResult->ID].executed = false;
            if (start != NULL && strncmp(start, serviceVars.ignoreUntil, (size_t) lengthOperator) != 0)
            {
                serviceVars.ignoreNextSubCmd = false;
            }
        }

        //PREPARE TO NEXT CYCLE
        free(tmpSubCmdResult); //Real result are on the pipeResult
        serviceVarsNext(&serviceVars);

        if (start != NULL && end != NULL)
        {
            getNextSubCommand(p, &start, &end);
            p = end + 1;
        }
    }

    //DO NOT REMOVE THIS CODE --Zanna_37--
    // ATTENDO TUTTI I GESTORI
    // non serve attenderli perché ci pensa pipeResult
    //pid_t pidFigli;
    //while ((pidFigli = waitpid(-1, NULL, 0)) != -1)
    //{
    //    DEBUG_PRINT("Process %d terminated\n", pidFigli);
    //}
    //END - DO NOT REMOVE THIS CODE --Zanna_37--


    //SAVING SUBCOMMANDS-RESULT
    close(pipeResult[WRITE]);
    SubCommandResult subCmdResult;

    while (w_read(pipeResult[READ], &subCmdResult, sizeof(SubCommandResult)) != 0)
    {
        cmd->subCommandResults[subCmdResult.ID] = subCmdResult;
    }
    close(pipeResult[READ]);

    //SEND COMMAND-RESULT
    send_msg(msqid, cmd);

    // FREEING DYNAMICALLY ALLOCATED MEMORY
    free(pipefds);
    for (i = 0; i < cmd->n_subCommands; i++)
    {
        //free(cmd->subCommandResults[i]);
    }
    free(cmd);
    // END FREEING DYNAMICALLY ALLOCATED MEMORY

    exitAndNotifyDaemon(EXIT_SUCCESS);

    //UNREACHABLE CODE
    return 1;
}
