#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <pwd.h>
#include <fcntl.h>
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
    operatorVars->outMode = MODE_FILEOVER;

    if (getcwd(operatorVars->currentDirectory, sizeof(operatorVars->currentDirectory)) == NULL)
    {
        // TODO: gestisci fallimento
    }
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

/**
 * Signal handler to notify daemon unexpected closure
 * @param signum Signal number
 */
void interrupt_sighandler(int signum)
{
    switch (signum)
    {
        case SIGTERM:
        case SIGQUIT:
            exitAndNotifyDaemon(EXIT_SUCCESS);
            break;
        case SIGINT:
            error_fatal(ERR_X, "Command not logged");
            break;
        case SIGUSR1:
            exitAndNotifyDaemon(EXIT_SUCCESS);
        case SIGUSR2:
            error_fatal(ERR_X, "The logging service encountered an error\nPlease check logs at \""DAEMON_ERRORFILE"\"");
            break;
        default:
            DEBUG_PRINT("Signal: %d\n", signum);
            exitAndNotifyDaemon(128 + signum);
    }
}

int main(int argc, char *argv[])
{
    pid_main = getpid();
    //TODO check del valore di ritorno
    msqid = createOrGetDaemon();

    int i;
    for (i = 1; i <= 64; i++)
    {
        if (i != SIGTSTP && i != SIGCONT && i != SIGCHLD)
        {
            signal(i, interrupt_sighandler);
        }
    }

    sanityCheck();
    Command *cmd = parseCommand(argc, argv);

    FlagRedirectVars flagVars;
    flagVars.output_mode = cmd->output_mode;
    strcpy(flagVars.output_path, cmd->output_path);
    flagVars.error_mode = cmd->error_mode;
    strcpy(flagVars.error_path, cmd->error_path);

    if (flagVars.output_mode == MODE_FILEOVER)
    {
        int fd = w_open(flagVars.output_path, O_WRONLY | O_CREAT | O_TRUNC, USER_PERMS);
        w_close(fd);
    }
    if (flagVars.error_mode == MODE_FILEOVER)
    {
        int fd = w_open(flagVars.error_path, O_WRONLY | O_CREAT | O_TRUNC, USER_PERMS);
        w_close(fd);
    }

    //USERNAME AND UID
    struct passwd *pws;
    pws = getpwuid(geteuid());
    strcpy(cmd->username, pws->pw_name);
    cmd->uid = pws->pw_uid;

    //CREAZIONE PIPES |
    int n_pipes = countPipes(cmd->command);
    int n_fds = 2 * n_pipes;
    int *pipefds = malloc(n_fds * sizeof(int)); //FREED IN end of main
    for (i = 0; i < n_fds; i += 2)
    {
        w_pipe(pipefds + i);
    }

    //CREAZIONE THREAD
    pthread_t *threads = malloc(n_pipes * sizeof(pthread_t)); //FREED IN end of main

    //ESECUZIONE SUBCOMANDI
    char *p = cmd->command;
    char *start = NULL;
    char *end = NULL;
    int lengthOperator;

    //variabili per operatori
    OperatorVars operatorVars;
    initOperatorVars(&operatorVars);

    // Controlla e setta eventuali direzioni di stdput ed stderror come specificato dai flags
    int null_fd = manageQuietMode(cmd);

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
        for (i = 0; i < NUM_REDIR_CHECKS; i++)
        {
            if (start != NULL && end != NULL)
            {
                lengthOperator = (end - start + 1) * sizeof(char);
                bool readRedirectOperator = false;
                if (strncmp(start, ">", (size_t) lengthOperator) == 0)
                {
                    operatorVars.outRedirect = true;
                    operatorVars.outMode = MODE_FILEOVER;
                    readRedirectOperator = true;
                    getNextSubCommand(p, &start, &end);
                    p = end + 1;
                    int lengthFile = (end - start + 1) * sizeof(char);
                    sprintf(operatorVars.outFile, "%.*s", lengthFile, start);
                }
                else if (strncmp(start, ">>", (size_t) lengthOperator) == 0)
                {
                    operatorVars.outRedirect = true;
                    operatorVars.outMode = MODE_FILEAPP;
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
                //nothing to do
            }
        } //else there is no operator
        //END - READ OPERATOR

        //Da qui in poi gli operatori sono stati filtrati quindi sono presenti solo programmi validi da eseguire
        cmd->n_subCommands++;
        if (!operatorVars.ignoreNextSubCmd)
        {
            tmpSubCmdResult->executed = true;
            executeSubCommand(tmpSubCmdResult, pipefds, n_pipes, threads, &operatorVars, &flagVars);
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
            memset(operatorVars.inFile, 0, MAX_STRING_LENGTH);
            memset(operatorVars.outFile, 0, MAX_STRING_LENGTH);
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

    //SUSPEND WAITING FOR DAEMON ACK
    sigset_t mask, oldmask;
    //Set up the mask of signals to temporarily block
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);

    //Wait for a signal to arrive
    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    while (true)
    {
        sigsuspend(&oldmask);
    }
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    //UNREACHABLE CODE
    return 1;
}
