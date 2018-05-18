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
#include "./daemon.h"

#define APPEND "a"

extern int errno;

int msqid;

void sighandler(int signum)
{
    switch (signum)
    {
    case SIGINT:
    case SIGTERM:
    case SIGQUIT:
        msgctl(msqid, IPC_RMID, NULL);
        exit(EXIT_SUCCESS);
        break;
    }
}

void core(int msqid_param)
{
    // Setto la variabile globale msqid per la gestione tramite sighandler
    msqid = msqid_param;

    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGQUIT, sighandler);

    // Variabili della logica: strutture dei due tipi di messaggi ricevibili, numero di processi in esecuzione
    stat_msg s_msg;
    proc_msg p_msg;
    int proc_count = 0;

    do
    {
        // Prova a ricevere un stat_msg
        msgrcv(msqid, &s_msg, STATSZ, STAT, IPC_NOWAIT);
        if (errno == ENOMSG)
        {
            errno = 0;
        }
        else
        {
            FILE *fp;
            fp = w_fopen(LOGFILE, APPEND);
            printStatsS(fp, &s_msg.sub);
            fclose(fp);
        }

        // Prova a ricevere un proc_msg di tipo PROC_INIT
        msgrcv(msqid, &p_msg, PROCSZ, PROC_INIT, IPC_NOWAIT);
        if (errno == ENOMSG)
        {
            errno = 0;
        }
        else
        {
            proc_count++;
        }

        // Prova  a ricevere un PROC_CLOSE
        msgrcv(msqid, &p_msg, PROCSZ, PROC_CLOSE, IPC_NOWAIT);
        if (errno == ENOMSG)
        {
            errno = 0;
        }
        else
        {
            proc_count--;
        }

    } while (proc_count > 0);
    // TODO: implementare un timeout per l'uscita dal ciclo?

    // Se esce dal ciclo, non ci sono piu' processi in esecuzione, quindi si termina automaticamente
    kill(getpid(), SIGTERM);
}
