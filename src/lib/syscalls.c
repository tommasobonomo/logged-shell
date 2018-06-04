#include "./syscalls.h"
#include "./errors.h"
#include "../daemon/daemon.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

extern pid_t pid_main;

pid_t w_fork()
{
    pid_t cid = fork();
    if (cid < 0)
    {
        error_fatal(ERR_SYSCALL, "Error during forking procedure", EXIT_FAILURE);
    }
    return cid;
}

sighandler_t w_signal(int signum, sighandler_t handler)
{
    sighandler_t oldSighandler = signal(signum, handler);
    if (oldSighandler == SIG_ERR)
    {
        char signum_str[4];
        sprintf(signum_str, "%d", signum);
        error_warning(ERR_SIGNAL, signum_str);
    }

    return oldSighandler;
}

int w_open(const char *pathname, int mode, mode_t permissions)
{
    int fd = open(pathname, mode, permissions);
    if (fd < 0)
    {
        error_fatal(ERR_IO_FILE, pathname, EXIT_FAILURE);
    }
    return fd;
}

FILE *w_fopen(const char *pathname, const char *mode)
{
    FILE *fp = fopen(pathname, mode);
    if (fp == NULL)
    {
        error_fatal(ERR_IO_FILE, pathname, EXIT_FAILURE);
    }

    return fp;
}

int w_dup2(int oldfd, int newfd)
{
    int result = dup2(oldfd, newfd);
    if (result < 0)
    {
        error_fatal(ERR_SYSCALL, "dup2 failed", EXIT_FAILURE);
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
        error_fatal(ERR_SYSCALL, "pipe failed", EXIT_FAILURE);
    }
    return result;
}

int w_execvp(const char *file, char *const argv[])
{
    int result = execvp(file, argv);
    if (result < 0)
    {
        error_fatal(ERR_EXEC, argv[0], EXIT_CMD_NOT_FOUND_FAILURE);
    }
    return result;
}

int w_msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg)
{
    int result = msgsnd(msqid, msgp, msgsz, msgflg);
    if (result < 0)
    {
        if (((proc_msg *) msgp)->type == TYPE_PROC_CLOSE)
            error_warning(ERR_SYSCALL, "msg queue seems yet closed");
        else
            error_fatal(ERR_SYSCALL, "msgsnd failed", EXIT_FAILURE);
    }
    return result;
}

int w_chdir(char *path)
{
    int result = chdir(path);
    if (result == -1)
    {
        error_fatal(ERR_SYSCALL, "changing directory failed", EXIT_FAILURE);
    }
    return result;
}

pid_t w_setsid()
{
    pid_t result = setsid();
    if (result < 0)
    {
        error_fatal(ERR_X, "New session and new process group failed", EXIT_FAILURE);
    }
    return result;
}

int w_mkdir(const char *pathname, mode_t mode)
{
    int result = mkdir(pathname, mode);
    if (result == -1)
    {
        if (errno != EEXIST)
        {
            error_fatal(ERR_SYSCALL, "mkdir failed", EXIT_FAILURE);
        }
    }
    return result;
}

void exitAndNotifyDaemon(int status)
{
    if (pid_main == getpid())
    {
        send_close(msqid);
    }
    exit(status);
}

pid_t w_wait4(pid_t pid, int *wstatus, int options, struct rusage *rusage)
{
    pid_t result = wait4(pid, wstatus, options, rusage);
    if (result == -1)
    {
        error_warning(ERR_SYSCALL, "wait4 failed");
    }
    return result;
}

char *w_getcwd(char *buf, size_t size)
{
    char *result = getcwd(buf, size);
    if (result == NULL)
    {
        error_fatal(ERR_SYSCALL, "getcwd failed", EXIT_FAILURE);
    }
    return result;
}

key_t w_ftok(const char *pathname, int proj_id)
{
    key_t result = ftok(pathname, proj_id);
    if (result == -1)
    {
        error_fatal(ERR_SYSCALL, "ftok failed", EXIT_FAILURE);
    }
    return result;
}
