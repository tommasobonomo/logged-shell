#include "./daemon.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include "../lib/commands.h"
#include "../lib/utilities.h"
#include "../lib/errors.h"
#include "../lib/syscalls.h"

int check()
{
    w_mkdir("/tmp/" TOOL_FOLDER, USER_AND_DAEMON_PERMS);

    key_t key = ftok(MSGQUE_PATH, MSGQUE_NUM); //scegliere percorso univovo
    DEBUG_PRINT("key: %d\n", key);

    int msqid = msgget(key, USER_PERMS | IPC_CREAT | IPC_EXCL);
    if (msqid >= 0)
    {
        daemonize(msqid);
    }
    else
    {
        msqid = msgget(key, USER_PERMS | IPC_CREAT);
        if (msqid < 0)
        {
            error_fatal(ERR_SYSCALL, "msgget failed");
        }
    }
    DEBUG_PRINT("msqid: %d\n", msqid);

    proc_msg init;
    init.type = PROC_INIT;
    w_msgsnd(msqid, &init, PROCSZ, 0);

    return msqid;
}

void send_msg(int msqid, Command *cmd)
{
    stat_msg msg;

    memset(&msg, 0, sizeof(stat_msg)); //initialize padding

    msg.type = STAT;
    msg.cmd = *cmd;

    w_msgsnd(msqid, &msg, COMMAND_SIZE, 0);
}

void send_close(int msqid)
{
    proc_msg close;
    close.type = PROC_CLOSE;
    w_msgsnd(msqid, &close, PROCSZ, 0);
}
