#include "syscalls.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/msg.h>

pid_t w_fork()
{
    pid_t fid = fork();
    if (fid < 0)
    {
        error_fatal(ERR_SYSCALL, "Error during forking procedure\n");
    }
    return fid;
}

sighandler_t w_signal(int signum, sighandler_t handler)
{
    sighandler_t oldSighandler = signal(signum, handler);
    if (oldSighandler == SIG_ERR)
    {
        error_fatal(ERR_SYSCALL, "can't catch signals");
    }

    return oldSighandler;
}

FILE *w_fopen(const char *pathname, const char *mode)
{
    FILE *fp = fopen(pathname, mode);
    if (fp == NULL)
    {
        error_fatal(ERR_IO_FILE, pathname);
    }

    return fp;
}

int w_dup2(int oldfd, int newfd)
{
    int result = dup2(oldfd, newfd);
    if (result < 0)
    {
        error_fatal(ERR_SYSCALL, "dup2 failed");
    }
    return result;
}

int w_close(int fd)
{
    int result = close(fd);
    if (result < 0)
    {
        error_warning(ERR_SYSCALL, "close failed");
    }
    return result;
}

int w_pipe(int pipefd[2])
{
    int result = pipe(pipefd);
    if (result < 0)
    {
        error_fatal(ERR_SYSCALL, "pipe failed");
    }
    return result;
}

int w_execvp(const char *file, char *const argv[])
{
    int result = execvp(file, argv);
    if (result < 0)
    {
        error_fatal(ERR_SYSCALL, "exec failed");
    }
    return result;
}

int w_msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg)
{
    int result = msgsnd(msqid, msgp, msgsz, msgflg);
    if (result < 0)
    {
        error_fatal(ERR_SYSCALL, "msgsnd failed");
    }
    return result;
}