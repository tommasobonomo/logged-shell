#ifndef DAEMON_H
#define DAEMON_H

#include "../lib/commands.h"

#define MSGQUE_PATH "."
#define MSGQUE_NUM 1
#define LOGFILE "/tmp/xlogfile.txt"

// Core.c
void daemon_sighandler(int signum);
void core(int msqid_param);

// Message
#define maxstr 80
#define msgsz maxstr * sizeof(char)
#define PERMS 0600
typedef struct message
{
	long type;		   // > 0
	char text[maxstr]; //payload
} message;

// API
int check();
void send_msg(int msqid, struct SubCommandResult *subres);

// GetDaemon
void daemonize(int msqid);

#endif