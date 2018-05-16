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

	int msqid = msgget(key, PERMS | IPC_CREAT);
	printf("msqid: %d\n", msqid);
	perror("Queue creation");

	msg msg1;
	msg1.type = 1;
	strcpy(msg1.text, "Hello world\0");
    printf("msg: %s\n", msg1.text);

	msgsnd(msqid, &msg1, msgsz, 0);
	return 0;
}
