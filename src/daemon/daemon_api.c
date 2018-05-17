#include "../lib/commands.h"
#include "daemon.h"
#include "../lib/utilities.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>

int check()
{
	key_t key = ftok(MSGQUE_PATH, MSGQUE_NUM); //scegliere percorso univovo
	DEBUG_PRINT("key: %d\n", key);

	int msqid = msgget(key, PERMS | IPC_CREAT | IPC_EXCL);
	if (msqid >= 0)
	{
		daemonize(msqid);
	}
	else
	{
		msqid = msgget(key, PERMS | IPC_CREAT);
	}
	DEBUG_PRINT("msqid: %d\n", msqid);

	return msqid;
}

void send_msg(int msqid, struct SubCommandResult *subres)
{
	message msg;
	msg.type = 1;
	strcpy(msg.text, "Hello world\0");

	msgsnd(msqid, &msg, msgsz, 0);
}