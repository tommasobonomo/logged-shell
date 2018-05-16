#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

int main(int argc, char const *argv[]) {
    key_t key = ftok(".", 1);
    printf("key: %d\n",key);
    int idq = msgget(key, IPC_CREAT | IPC_EXCL);
    perror("Queue creation");44

    return 0;
}
