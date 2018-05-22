#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>
#include <fcntl.h>
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
    DEBUG_PRINT("  ###########\n");
    DEBUG_PRINT("  ## DEBUG ##\n");
    DEBUG_PRINT("  ###########\n\n");

    pid_main = getpid();
    DEBUG_PRINT("pid: %d\n\n", pid_main);

    //SANITY CHECKS
    if (PIPE_BUF < sizeof(SubCommandResult))
    {
        fprintf(stderr, "FATAL ERROR!!!\n");
        fprintf(stderr, "PIPE_BUF max size: %d\n", PIPE_BUF);
        fprintf(stderr, "Struct SubCommandResult size: %d\n", (int)sizeof(SubCommandResult));
        exitAndNotifyDaemon(EXIT_FAILURE);
    }

    DEBUG_PRINT("PIPE_BUF max size: %d\n", PIPE_BUF);
    DEBUG_PRINT("Struct SubCommandResult size: %d\n", (int)sizeof(SubCommandResult));

    FILE *msgmaxFd = w_fopen("/proc/sys/kernel/msgmax", "r");
    unsigned int msgmax;
    fscanf(msgmaxFd, "%d", &msgmax);
    fclose(msgmaxFd);

    if (msgmax < sizeof(Command))
    {
        fprintf(stderr, "FATAL ERROR!!!\n");
        fprintf(stderr, "MSGMAX max size: %d\n", msgmax);
        fprintf(stderr, "Struct Command size: %d\n", (int)sizeof(Command));
        exitAndNotifyDaemon(EXIT_FAILURE);
    }

    DEBUG_PRINT("MSGMAX max size: %d\n", msgmax);
    DEBUG_PRINT("Struct Command size: %d\n", (int)sizeof(Command));

    //END - SANITY CHECKS

    // Comunicazione iniziale con demone, va fatta all'inizio dell'esecuzione
    msqid = check();

    int i = 1;
    for (; i <= 64; i++)
    {
        if (i != SIGCONT && i != SIGCHLD)
        {
            signal(i, interrupt_sighandler);
        }
    }

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
    bool prevPipe = false;
    bool nextPipe = false;
    bool nextAnd = false;
    bool nextOr = false;
    int pipeIndex = 0;

    // Controlla se stampare o no a video
    int null_fd = -1;
    if (cmd->output_mode == MODE_DISCARD)
    {
        null_fd = w_open("/dev/null", O_WRONLY);
        dup2(null_fd, STDOUT_FILENO);
    }

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
            int lengthOperator = (end - start) * sizeof(*start) + 1;
            if (strncmp(start, "|", (size_t)lengthOperator) == 0)
            {
                nextPipe = true;
                //TODO redir output su fd corrente
            }
            else if (strncmp(start, "&&", (size_t)lengthOperator) == 0)
            {
                nextAnd = true;
            }
            else if (strncmp(start, "||", (size_t)lengthOperator) == 0)
            {
                nextOr = true;
            }
            else if (strncmp(start, ";", (size_t)lengthOperator) == 0)
            {
                //fare niente
            }
        } //else there is no operator
        //END - READ OPERATOR

        tmpSubCmdResult->ID = cmd->n_subCommands++;
        executeSubCommand(tmpSubCmdResult, pipeResult, pipefds, n_pipes, pipeIndex, prevPipe, nextPipe, nextAnd,
                          nextOr);

        //PREPARE TO NEXT CYCLE
        free(tmpSubCmdResult); //Real result are on the pipeResult
        if (nextPipe == true)
        {
            pipeIndex++;
        }
        prevPipe = nextPipe;
        nextPipe = false;
        nextAnd = false;
        nextOr = false;

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
