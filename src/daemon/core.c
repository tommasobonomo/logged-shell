#include "message.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>

int main(int argc, char const *argv[]) {
	key_t key = ftok(".", 1);
	printf("key: %d\n", key);

	int msqid = msgget(key, PERMS | IPC_CREAT); //meglio se passato
	printf("msqid: %d\n", msqid);
	perror("Queue creation");

	msg msg1;
    msgrcv(msqid, &msg1, msgsz, 1, 0);
    perror("?");

    printf("msg: %s\n", msg1.text);
	//msgctl(msqid, IPC_RMID, NULL);
	return 0;
}
