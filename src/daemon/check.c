#include "check.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

int main(int argc, char const *argv[]) {
    key_t key = ftok(".", 1);
    printf("key: %d\n",key);
    msgget(key, IPC_CREAT | IPC_EXCL);
    perror("Queue creation");

    int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);


    return 0;
}
