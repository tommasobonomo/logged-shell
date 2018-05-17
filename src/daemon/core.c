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

// Riguardare bene comportamento
void daemon_sighandler(int signum)
{
	switch (signum)
	{
	case SIGINT:
	case SIGTERM:
		DEBUG_PRINT("Caught signal %d, coming out...\n", signum);
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

	signal(SIGINT, daemon_sighandler);

	message msg;
	while (1)
	{
		msgrcv(msqid, &msg, msgsz, 1, 0);
		DEBUG_PRINT("msg: %s\n", msg.text);

		// Da implementare kill demone e coda
	}
}
