#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <sys/types.h>
#include <sys/resource.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

typedef void (*sighandler_t)(int);

pid_t w_fork();

sighandler_t w_signal(int signum, sighandler_t handler);

int w_open(const char *pathname, int mode, mode_t permissions);

FILE *w_fopen(const char *pathname, const char *mode);

int w_dup2(int oldfd, int newfd);

int w_close(int fd);

int w_pipe(int pipefd[2]);

int w_execvp(const char *file, char *const argv[]);

int w_msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);

int w_chdir(char *path);

pid_t w_setsid();

int w_mkdir(const char *pathname, mode_t mode);

void exitAndNotifyDaemon(int status);

pid_t w_wait4(pid_t pid, int *wstatus, int options,
              struct rusage *rusage);

char *w_getcwd(char *buf, size_t size);

key_t w_ftok(const char *pathname, int proj_id);

#endif
