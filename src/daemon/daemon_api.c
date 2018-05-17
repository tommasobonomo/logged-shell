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
		proc_msg init;
		init.type = PROC_INIT;
		msgsnd(msqid, &init, PROCSZ, 0);
		daemonize(msqid);
	}
	else
	{
		msqid = msgget(key, PERMS | IPC_CREAT);
		proc_msg init;
		init.type = PROC_INIT;
		msgsnd(msqid, &init, PROCSZ, 0);
	}
	DEBUG_PRINT("msqid: %d\n", msqid);

	return msqid;
}

void send_msg(int msqid, struct SubCommandResult *subres)
{
	stat_msg msg;
	msg.type = STAT;
	msg.sub = *subres;

	msgsnd(msqid, &msg, STATSZ, 0);
}

void send_close(int msqid)
{
	proc_msg close;
	close.type = PROC_CLOSE;
	msgsnd(msqid, &close, PROCSZ, 0);
}