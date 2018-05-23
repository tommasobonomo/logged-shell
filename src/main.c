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
#define NUM_REDIR_CHECKS 2

int msqid;
pid_t pid_main;

void initOperatorVars(OperatorVars *operatorVars)
{
    operatorVars->pipeIndex = 0;
    operatorVars->prevPipe = false;
    operatorVars->nextPipe = false;
    operatorVars->nextAnd = false;
    operatorVars->nextOr = false;
    operatorVars->ignoreNextSubCmd = false;
    operatorVars->ignoreUntil[0] = '\0';

    operatorVars->inRedirect = false;
    operatorVars->outRedirect = false;
    operatorVars->inFile[0] = '\0';
    operatorVars->outFile[0] = '\0';
}

void OperatorVarsNext(OperatorVars *operatorVars)
{
    if (operatorVars->nextPipe == true)
    {
        operatorVars->pipeIndex++;
    }
    operatorVars->prevPipe = operatorVars->nextPipe;
    operatorVars->nextPipe = false;

    operatorVars->nextAnd = false;
    operatorVars->nextOr = false;
}

// Handler di segnali per mandare un segnale di chiusura al demone comunque
void interrupt_sighandler(int signum)
{
    switch (signum)
    {
        case SIGINT:
            fprintf(stderr, "\n(Command not logged)\n");
        case SIGTERM:
        case SIGQUIT:
            exitAndNotifyDaemon(EXIT_SUCCESS);
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
        if (i != SIGTSTP && i != SIGCONT && i != SIGCHLD)
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
    
    //variabili per operatori
    OperatorVars operatorVars;
    initOperatorVars(&operatorVars);

    // Controlla le direzioni di tutti output e error a seconda dei flag
    int null_fd = setNullRedirections(cmd);

    getNextSubCommand(p, &start, &end);
    p = end + 1;

    while (start != NULL && end != NULL)
    {
        SubCommandResult *tmpSubCmdResult = malloc(sizeof(SubCommandResult));

        int length = (end - start) * sizeof(*start) + 1;
        sprintf(tmpSubCmdResult->subCommand, "%.*s", length, start);

        // Leggo operatore o redir
        getNextSubCommand(p, &start, &end);
        p = end + 1;

        // Controllo due volte se l'operatore è < o >, altrimenti leggo operatore
        int i;

        for (i = 0; i < NUM_REDIR_CHECKS; i++)
        {
            if (start != NULL && end != NULL)
            {
                lengthOperator = (end - start) * sizeof(*start) + 1;
                bool readRedirectOperator = false;
                if (strncmp(start, ">", (size_t)lengthOperator) == 0)
                {
                    operatorVars.outRedirect = true;
                    readRedirectOperator = true;
                    getNextSubCommand(p, &start, &end);
                    p = end + 1;
                    int lengthFile = (end - start) * sizeof(*start) + 1;
                    sprintf(operatorVars.outFile, "%.*s", lengthFile, start);
                }
                else if (strncmp(start, "<", (size_t)lengthOperator) == 0)
                {
                    operatorVars.inRedirect = true;
                    readRedirectOperator = true;
                    getNextSubCommand(p, &start, &end);
                    p = end + 1;
                    int lengthFile = (end - start) * sizeof(*start) + 1;
                    sprintf(operatorVars.inFile, "%.*s", lengthFile, start);
                }

                if (readRedirectOperator)
                {
                    getNextSubCommand(p, &start, &end);
                    p = end + 1;
                }
            }
        }

        // Leggo operatore
        if (start != NULL && end != NULL)
        {
            lengthOperator = (end - start) * sizeof(*start) + 1;

            if (strncmp(start, "|", (size_t) lengthOperator) == 0)
            {
                operatorVars.nextPipe = true;
            }
            else if (strncmp(start, "&&", (size_t) lengthOperator) == 0)
            {
                operatorVars.nextAnd = true;
            }
            else if (strncmp(start, "||", (size_t) lengthOperator) == 0)
            {
                operatorVars.nextOr = true;
            }
            else if (strncmp(start, ";", (size_t) lengthOperator) == 0)
            {
                //fare niente
            }
        } //else there is no operator
          //END - READ OPERATOR

        tmpSubCmdResult->ID = cmd->n_subCommands++;
        if (!operatorVars.ignoreNextSubCmd)
        {
            tmpSubCmdResult->executed = true;
            executeSubCommand(tmpSubCmdResult, pipeResult, pipefds, n_pipes, &operatorVars);
        }
        else
        {
            tmpSubCmdResult->executed = false;
            cmd->subCommandResults[tmpSubCmdResult->ID] = *tmpSubCmdResult;
            if (start != NULL && strncmp(start, operatorVars.ignoreUntil, (size_t) lengthOperator) != 0)
            {
                operatorVars.ignoreNextSubCmd = false;
            }
        }

        // Reset redirections
        if (operatorVars.outRedirect || operatorVars.inRedirect)
        {
            operatorVars.inRedirect = false;
            operatorVars.outRedirect = false;
            memset(operatorVars.inFile, 0, MAX_STRING_LENGHT);
            memset(operatorVars.outFile, 0, MAX_STRING_LENGHT);
        }

        //PREPARE TO NEXT CYCLE
        free(tmpSubCmdResult); //Real result are on the pipeResult
        OperatorVarsNext(&operatorVars);

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

    // Chiudo eventuale ridirezione output
    if (null_fd != -1)
    {
        close(null_fd);
    }

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
