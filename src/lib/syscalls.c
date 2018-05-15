#include "syscalls.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

pid_t frk()
{
    pid_t fid = fork();
    if (fid < 0)
    {
        error_fatal(ERR_X, "Error during forking procedure\n");
    }
    return fid;
}

sighandler_t wSignal(int signum, sighandler_t handler)
{
    sighandler_t oldSighandler = signal(signum, handler);
    if (oldSighandler == SIG_ERR)
    {
        error_fatal(ERR_X, "can't catch signals");
    }

    return oldSighandler;
}

FILE *wFopen(const char *restrict pathname, const char *restrict mode)
{
    FILE *fp = fopen(pathname, mode); //TODO fare wrapper
    if (fp == NULL)
    {
        error_fatal(ERR_IO_FILE, pathname);
    }

    return fp;

}