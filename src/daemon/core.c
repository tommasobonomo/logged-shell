#include "./daemon.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include "../lib/utilities.h"
#include "../lib/syscalls.h"
#include "../statistics/statHelper.h"

#define APPEND "a"

extern pid_t pid_main; //TODO STRONZATA ENORME

void sighandler(int signum)
{
    msgctl(msqid, IPC_RMID, NULL);
    exit(128 + signum);
}

void manageDaemonError(char const *error_msg, char const *secondary_msg, FILE *error_fd)
{
    time_t now = time(NULL);
    struct tm nowFormatted = *localtime(&now);
    fprintf(error_fd, "%d-%02d-%02d %02d:%02d:%02d\n"
                      "  ERROR: %s %s --> %s\n",
            nowFormatted.tm_year + 1900, nowFormatted.tm_mon + 1, nowFormatted.tm_mday, nowFormatted.tm_hour,
            nowFormatted.tm_min, nowFormatted.tm_sec,
            error_msg, secondary_msg, strerror(errno));
    fclose(error_fd);

    kill(pid_main, SIGUSR2);

    msgctl(msqid, IPC_RMID, NULL);
    exit(37); //TODO cambia
}

void manageDaemonErrorFile()
{
    msgctl(msqid, IPC_RMID, NULL);
    exit(37); //TODO cambia
}

void core(int msqid_param)
{
    // Setto la variabile globale msqid per la gestione tramite sighandler
    msqid = msqid_param;

    int i = 1;
    for (; i <= 64; i++)
    {
        if (i != SIGTSTP && i != SIGCONT && i != SIGCHLD)
        {
            signal(i, sighandler);
        }
    }

    // Variabili della logica: strutture dei due tipi di messaggi ricevibili, numero di processi in esecuzione
    stat_msg s_msg;
    proc_msg p_msg;
    int proc_count = 0;

    FILE *error_fd = fopen(DAEMON_ERRORFILE, APPEND);
    if (error_fd == NULL)
    {
        manageDaemonErrorFile();
    }

    do
    {
        int result = msgrcv(msqid, &p_msg, PROCSZ, 0, 0);

        if (errno == E2BIG)
        {
            // C'è almeno una statistica da leggere
            result = msgrcv(msqid, &s_msg, COMMAND_SIZE, STAT, 0);
            if (result < 0)
            {
                manageDaemonError("msgrcv statistic failed", NULL, error_fd);
            }

            FILE *fp;
            fp = fopen(s_msg.cmd.log_path, APPEND);
            if (fp == NULL)
            {
                // Fallita creazione file
                manageDaemonError("failed to create logfile", s_msg.cmd.log_path, error_fd);
            }

            Command cmd;
            cmd = s_msg.cmd;
            printStatsCommand(fp, &cmd);

            fclose(fp);
            errno = 0;
            kill(pid_main, SIGUSR1);
        }
        else
        {
            if (result < 0)
            {
                manageDaemonError("msgrcv signal failed", NULL, error_fd);
            }

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
    exit(EXIT_SUCCESS);
}
