#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>
#include "./lib/syscalls.h"
#include "./lib/commands.h"
#include "./parser/parser.h"
#include "./executer/executer.h"
#include "./daemon/daemon.h"
#include "./lib/errors.h"

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
        case SIGTERM:
        case SIGQUIT:
            exitAndNotifyDaemon(EXIT_SUCCESS);
            break;
        case SIGINT:
            fprintf(stderr, "\n(Command not logged)\n");
            exitAndNotifyDaemon(128 + signum);
            break;
        default:
            DEBUG_PRINT("Signal: %d\n", signum);
            exitAndNotifyDaemon(128 + signum);
    }
}

int main(int argc, char *argv[])
{
    pid_main = getpid();
    //TODO vedi il valore di ritorno
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
    int n_fds = 2 * n_pipes;
    int *pipefds = malloc(n_fds * sizeof(int));
    for (i = 0; i < n_fds; i += 2)
    {
        w_pipe(pipefds + i);
    }

    //CREAZIONE THREAD
    pthread_t *threads = malloc(n_pipes * sizeof(pthread_t));

    //ESECUZIONE SUBCOMANDI
    char *p = cmd->command;
    char *start = NULL;
    char *end = NULL;
    int lengthOperator;

    //variabili per operatori
    OperatorVars operatorVars;
    initOperatorVars(&operatorVars);

    // Controlla e setta eventuali direzioni di stdput ed stderror come specificato dai flags
    int null_fd = setNullRedirections(cmd);

    if (cmd->command[0] == '\0')
    {
        error_fatal(ERR_BAD_ARG_X, "Command to execute not specified");
    }

    getNextSubCommand(p, &start, &end);
    p = end + 1;

    while (start != NULL && end != NULL)
    {
        SubCommandResult *tmpSubCmdResult = &cmd->subCommandResults[cmd->n_subCommands];

        int length = (end - start + 1) * sizeof(char);
        sprintf(tmpSubCmdResult->subCommand, "%.*s", length, start);

        // Leggo operatore o redir
        getNextSubCommand(p, &start, &end);
        p = end + 1;

        // Controllo NUM_REDIR_CHECKS volte se l'operatore è < o >, altrimenti leggo operatore
        for (i = 0; i < NUM_REDIR_CHECKS; i++) //TODO perchè NUM_REDIR_CHECKS = 2?
        {
            if (start != NULL && end != NULL)
            {
                lengthOperator = (end - start + 1) * sizeof(char);
                bool readRedirectOperator = false;
                if (strncmp(start, ">", (size_t) lengthOperator) == 0)
                {
                    operatorVars.outRedirect = true;
                    readRedirectOperator = true;
                    getNextSubCommand(p, &start, &end);
                    p = end + 1;
                    int lengthFile = (end - start + 1) * sizeof(char);
                    sprintf(operatorVars.outFile, "%.*s", lengthFile, start);
                }
                else if (strncmp(start, "<", (size_t) lengthOperator) == 0)
                {
                    operatorVars.inRedirect = true;
                    readRedirectOperator = true;
                    getNextSubCommand(p, &start, &end);
                    p = end + 1;
                    int lengthFile = (end - start + 1) * sizeof(char);
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
            lengthOperator = (end - start + 1) * sizeof(char);

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

        //Da qui in poi gli operatori sono stati filtrati quindi sono presenti solo programmi validi da eseguire
        cmd->n_subCommands++;
        if (!operatorVars.ignoreNextSubCmd)
        {
            tmpSubCmdResult->executed = true;
            executeSubCommand(tmpSubCmdResult, pipefds, n_pipes, threads, &operatorVars);
        }
        else
        {
            tmpSubCmdResult->executed = false;
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
        OperatorVarsNext(&operatorVars);

        if (start != NULL && end != NULL)
        {
            getNextSubCommand(p, &start, &end);
            p = end + 1;
        }
    }
//DO NOT REMOVE THIS CODE - Zanna_37 -
//     //ATTENDO TUTTI I FIGLI
//    pid_t pidFigli;
//    while ((pidFigli = waitpid(-1, NULL, 0)) != -1)
//    {
//        DEBUG_PRINT("terminato figlio %d\n", pidFigli);
//    }
//END - DO NOT REMOVE THIS CODE - Zanna_37 -

    // Chiudo eventuale ridirezione output
    if (null_fd != -1)
    {
        close(null_fd);
    }

    //SEND COMMAND-RESULT
    send_msg(msqid, cmd);

    // FREEING DYNAMICALLY ALLOCATED MEMORY
    free(pipefds);
    free(threads);
    free(cmd);
    // END FREEING DYNAMICALLY ALLOCATED MEMORY

    exitAndNotifyDaemon(EXIT_SUCCESS);
    //UNREACHABLE CODE
    return 1;
}
