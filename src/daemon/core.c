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

void sighandler(int signum)
{
    msgctl(msqid, IPC_RMID, NULL);
    exit(EXIT_FATAL_SIGNAL + signum);
}

void daemonLog(char const *error_msg, FILE *daemon_internal_log_fd)
{
    time_t now = time(NULL);
    struct tm nowFormatted = *localtime(&now);
    fprintf(daemon_internal_log_fd, "%d-%02d-%02d %02d:%02d:%02d > "
                      "LOG: %s\n",
            nowFormatted.tm_year + 1900, nowFormatted.tm_mon + 1, nowFormatted.tm_mday, nowFormatted.tm_hour,
            nowFormatted.tm_min, nowFormatted.tm_sec,
            error_msg);
}

void manageDaemonError(char const *error_msg, char const *secondary_msg, FILE *daemon_internal_log_fd, pid_t pid_main)
{
    time_t now = time(NULL);
    struct tm nowFormatted = *localtime(&now);
    fprintf(daemon_internal_log_fd, "%d-%02d-%02d %02d:%02d:%02d > "
                      "ERR: %s %s --> %s\n",
            nowFormatted.tm_year + 1900, nowFormatted.tm_mon + 1, nowFormatted.tm_mday, nowFormatted.tm_hour,
            nowFormatted.tm_min, nowFormatted.tm_sec,
            error_msg, secondary_msg, strerror(errno));


    if (pid_main == PID_MAIN_UNKNOWN)
    {
        fclose(daemon_internal_log_fd);
        msgctl(msqid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }
    else
    {
        kill(pid_main, SIGUSR2);
    }
}

void core(int msqid_param, FILE *daemon_internal_log_fd)
{
    // Setto la variabile globale msqid per la gestione tramite sighandler
    msqid = msqid_param;

    //Catch signals
    int i = 1;
    for (; i <= 64; i++)
    {
        switch (i)
        {
            case SIGKILL:
            case SIGCHLD:
            case SIGCONT:
            case SIGSTOP:
            case SIGTSTP:
            case 32:
            case 33:
                break;
            default:
                signal(i, sighandler);
                break;
        }
    }

    // Variabili della logica: strutture dei due tipi di messaggi ricevibili, numero di processi in esecuzione
    stat_msg s_msg;
    proc_msg p_msg;
    int proc_count = 0;

    do
    {
        ssize_t result = msgrcv(msqid, &p_msg, PROC_SIZE, 0, 0);

        if (errno == E2BIG)
        {
            // C'è almeno una statistica da leggere
            result = msgrcv(msqid, &s_msg, COMMAND_SIZE, TYPE_STAT, 0);
            if (result < 0)
            {
                manageDaemonError("msgrcv statistic failed", NULL, daemon_internal_log_fd, PID_MAIN_UNKNOWN);
            }
            else
            {
                Command cmd;
                cmd = s_msg.cmd;

                FILE *fp;
                fp = fopen(s_msg.cmd.log_path, APPEND);
                if (fp == NULL)
                {
                    manageDaemonError("failed to create logfile", s_msg.cmd.log_path, daemon_internal_log_fd, cmd.pid_main);
                }
                else
                {
                    printStatsCommand(fp, &cmd);

                    fclose(fp);
                    errno = 0; //Reset errno
                    kill(cmd.pid_main, SIGUSR1);
                }
            }
        }
        else
        {
            if (result < 0)
            {
                manageDaemonError("msgrcv signal failed", NULL, daemon_internal_log_fd, PID_MAIN_UNKNOWN);
            }
            else
            {
                if (p_msg.type == TYPE_PROC_INIT)
                {
                    proc_count++;
                }
                else if (p_msg.type == TYPE_PROC_CLOSE)
                {
                    proc_count--;
                }
            }
        }
    } while (proc_count > 0);

    daemonLog("DAEMON SHUTDOWN\n", daemon_internal_log_fd);

    // Se esce dal ciclo, non ci sono piu' processi in esecuzione, quindi si termina automaticamente
    fclose(daemon_internal_log_fd);
    msgctl(msqid, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}
