#include "syscalls.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

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

FILE *w_fopen(const char *restrict pathname, const char *restrict mode)
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
    if (dup2(oldfd, newfd) < 0)
    {
        error_fatal(ERR_SYSCALL, "dup2 failed");
    }
}

int w_close(int fd)
{
    if (close(fd) < 0)
    {
        error_fatal(ERR_SYSCALL, "close failed");
    }
}

int w_pipe(int pipefd[2])
{
    if (pipe(pipefd) < 0)
    {
        error_fatal(ERR_SYSCALL, "pipe failed");
    }
}

int w_execvp(const char *file, char *const argv[])
{
    if (execvp(file, argv) < 0)
    {
        error_fatal(ERR_SYSCALL, "exec failed");
    }
}