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

void sighandler(int signum)
{
    //TODO si potrebbe aggiungere le informazioni di chiusura forzata al file di log
<<<<<<< HEAD
    exit(128+signum);
=======
    switch (signum)
	{
    	case SIGTERM:
    	case SIGQUIT:
    		exit(EXIT_SUCCESS);
    	case SIGINT:
    		exit(128+signum);
    	default:
    		exit(128+signum);
	}
>>>>>>> a9128aca29d9a4e6122fc7953ec523964a2332b2
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

    do
    {
        msgrcv(msqid, &p_msg, PROCSZ, 0, 0);

        if (errno == E2BIG)
        {
            // C'è almeno una statistica da leggere
            msgrcv(msqid, &s_msg, COMMAND_SIZE, STAT, 0);
            FILE *fp;
            fp = w_fopen(s_msg.cmd.log_path, "a");

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
    msgctl(msqid, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}
