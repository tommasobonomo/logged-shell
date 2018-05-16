#include "message.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>

int main(int argc, char const *argv[]) {
	key_t key = ftok(".", 1); //scegliere percorso univovo
	printf("key: %d\n", key);

	int msqid = msgget(key, PERMS | IPC_CREAT | IPC_EXCL);
	if (msqid >= 0) {
		// call core
	} else {
		msqid = msgget(key, PERMS | IPC_CREAT);
	}
	printf("msqid: %d\n", msqid);

	message msg;
	msg.type = 1;
	strcpy(msg.text, "Hello world\0");

	msgsnd(msqid, &msg, msgsz, 0);

	return 0;
}
