#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

typedef void (*sighandler_t)(int);

pid_t frk();

sighandler_t wSignal(int signum, sighandler_t handler);

FILE *wFopen(const char *restrict pathname, const char *restrict mode);

#endif