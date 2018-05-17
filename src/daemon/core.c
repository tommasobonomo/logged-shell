#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "../lib/utilities.h"
#include "daemon.h"

extern int errno;

int msqid;

// Riguardare bene comportamento
void sighandler(int signum)
{
	switch (signum)
	{
	case SIGINT:
	case SIGTERM:
		// // DEBUG
		// fp = fopen(LOGFILE, "w");
		// fprintf(fp, "msqid: %d\nsignum: %d\n", msqid, signum);
		// fclose(fp);
		// // END DEBUG

		msgctl(msqid, IPC_RMID, NULL);
		exit(EXIT_SUCCESS);
		break;
	}
}

void core(int msqid_param)
{
	msqid = msqid_param;

	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);

	stat_msg s_msg;
	proc_msg p_msg;
	int proc_count = 0;

	do
	{

		msgrcv(msqid, &s_msg, STATSZ, STAT, IPC_NOWAIT);
		if (errno == ENOMSG)
		{
			errno = 0;
		}
		else
		{
			FILE *fp;
			fp = fopen(LOGFILE, "w");
			fprintf(fp, "%s\n", s_msg.text);
			fclose(fp);
		}

		msgrcv(msqid, &p_msg, PROCSZ, PROC_INIT, IPC_NOWAIT);
		if (errno == ENOMSG)
		{
			errno = 0;
		}
		else
		{
			proc_count++;
		}

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

	kill(getpid(), SIGTERM);
}
