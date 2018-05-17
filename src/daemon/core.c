#include "message.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int msqid;

void sighandler(int signum) {
	switch (signum) {
		case SIGINT:
		case SIGTERM:
			printf("Caught signal %d, coming out...\n", signum);
			msgctl(msqid, IPC_RMID, NULL);
			perror("?");
			exit(1);
			break;
		case SIGQUIT:
			break;
	}
}

int main(int argc, char const *argv[]) {
	if (argc == 2) {
		signal(SIGINT, sighandler);
		msqid = atoi(argv[1]);
		printf("msqid: %d\n", msqid);

		message msg;
		while (1) {
		    msgrcv(msqid, &msg, msgsz, 1, 0);
			printf("msg: %s\n", msg.text);
		}

		msgctl(msqid, IPC_RMID, NULL);
		return 0;
	} else {
		return 1;
	}
}
