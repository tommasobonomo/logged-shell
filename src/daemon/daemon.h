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
#define MAXSTR 80
#define STATSZ MAXSTR * sizeof(char)
#define PERMS 0600

#define PROCSZ 0

#define STAT 1
#define PROC_INIT 2
#define PROC_CLOSE 3
typedef struct stat_msg
{
	long type;		   // > 0
	char text[MAXSTR]; //payload, sara' struttura del subcommandresult
} stat_msg;
typedef struct proc_msg
{
	long type;
} proc_msg;

// API
/**
 * Controlla se è presente una coda di messaggi, se si restituisce il suo 
 * identificativo (sapendo che il demone è in esevuzione), altrimenti 
 * crea una nuova coda di messaggi e inizializza il demone.
 * Comunica al demone che vi è una nuova istanza del tool in esecuzione.
 * @return l'identificativo della coda di messaggi, nuova o già presente.
 */
int check();

/**
 * Manda la struttura SubCommandResult come messaggio alla coda di messaggi
 * letta dal demone.
 * @param msqid l'identificativo della coda di messaggi
 * @param subres il puntatore alla struttura del sottocomando
 */
void send_msg(int msqid, struct SubCommandResult *subres);

void send_close(int msqid);

// GetDaemon
void daemonize(int msqid);

#endif