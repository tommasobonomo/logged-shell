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

int msqid;
FILE *fp;

// Riguardare bene comportamento
void sighandler(int signum)
{
	switch (signum)
	{
	case SIGINT:
	case SIGTERM:
		// DEBUG
		fp = fopen(LOGFILE, "w");
		fprintf(fp, "msqid: %d\nsignum: %d\n", msqid, signum);
		fclose(fp);
		// END DEBUG

		msgctl(msqid, IPC_RMID, NULL);
		exit(EXIT_SUCCESS);
		break;
	case SIGQUIT:
		break;
	}
}

void core(int msqid_param)
{
	msqid = msqid_param;

	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);
	signal(SIGQUIT, sighandler);

	message msg;
	while (1)
	{
		msgrcv(msqid, &msg, msgsz, 1, 0);

		// FILE *fp;
		fp = fopen(LOGFILE, "w");
		fprintf(fp, "%s\n", msg.text);
		fclose(fp);

		// DEBUG_PRINT("msg: %s\n", msg.text);

		// Da implementare kill demone e coda
	}
}
