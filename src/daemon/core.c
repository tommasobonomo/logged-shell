#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "../lib/utilities.h"
#include "../lib/syscalls.h"
#include "../statistics/statHelper.h"
#include "daemon.h"

#define APPEND "a"

void sighandler(int signum)
{
    switch (signum)
    {
    case SIGINT:
    case SIGTERM:
    case SIGQUIT:
        exitAndNotifyDaemon(EXIT_SUCCESS);
        break;
    default:
        exitAndNotifyDaemon(EXIT_FAILURE);
    }
}

void core(int msqid_param)
{
    // Setto la variabile globale msqid per la gestione tramite sighandler
    msqid = msqid_param;

    int i = 1;
    for (; i <= 64; i++)
    {
        if (i != SIGCONT && i != SIGCHLD)
        {
            signal(i, sighandler);
        }
    }

    // Variabili della logica: strutture dei due tipi di messaggi ricevibili, numero di processi in esecuzione
    stat_msg s_msg;
    proc_msg p_msg;
    int proc_count = 0;

    FILE *error_fd = w_fopen(DAEMON_ERRORFILE, APPEND);

    do
    {
        w_msgrcv(msqid, &p_msg, PROCSZ, 0, 0);

        if (errno == E2BIG)
        {
            // C'è almeno una statistica da leggere
            int result = msgrcv(msqid, &s_msg, COMMAND_SIZE, STAT, 0);
            if (result < 0)
            {
                fprintf(error_fd, "ERROR: msgrcv failed\n%s\n", strerror(errno));
                msgctl(msqid, IPC_RMID, NULL);
                exitAndNotifyDaemon(EXIT_FAILURE);
            }

            FILE *fp;
            fp = fopen(s_msg.cmd.log_path, APPEND);
            if (fp == NULL)
            {
                // Fallita creazione file
                fprintf(error_fd, "ERROR: failed to create logfile %s!\n", s_msg.cmd.log_path);
                msgctl(msqid, IPC_RMID, NULL);
                exitAndNotifyDaemon(EXIT_FAILURE);
            }

            Command cmd;
            cmd = s_msg.cmd;
            printStatsCommand(fp, &cmd);

            fclose(fp);
            errno = 0;
        }
        else
        {
            if (p_msg.type == PROC_INIT)
            {
                proc_count++;
            }
            else if (p_msg.type == PROC_CLOSE)
            {
                proc_count--;
            }
        }
    } while (proc_count > 0);

    // Se esce dal ciclo, non ci sono piu' processi in esecuzione, quindi si termina automaticamente
    fclose(error_fd);
    msgctl(msqid, IPC_RMID, NULL);
    exitAndNotifyDaemon(EXIT_SUCCESS);
}
