#ifndef DAEMON_H
#define DAEMON_H

#include <stdio.h>
#include "../lib/commands.h"

#define PID_MAIN_UNKNOWN (-1)
#define APPEND "a"

#define MSGQUE_PATH "/tmp/"TOOL_FOLDER      // Path per la msg_queue, impostato di default alla directory corrente
#define MSGQUE_NUM 37
// ID numerico della msg_queue, impostato di default a 1
#define DEFAULT_LOGPATH_TXT "/tmp/" TOOL_FOLDER "/default.txt" // Path del file di log. TODO: aggiungere l'opzione per customizzarlo
#define DEFAULT_LOGPATH_CSV "/tmp/" TOOL_FOLDER "/default.csv" // Path del file di log. TODO: aggiungere l'opzione per customizzarlo
#define DAEMON_INTERNAL_LOGFILE "/tmp/" TOOL_FOLDER "/daemon_internal.log"
#define DAEMON_INTERNAL_LOGFILE2 "/tmp/" TOOL_FOLDER "/daemon_internal2.log"

int msqid; // ID msg_queue creata ed attiva

// Core.c
/**
 * Esegue la logica del demone sulla msg_queue msqid_param: legge messaggi passati, decidendo anche quando terminare
 * @param msqid_param la msg_queue sulla dalla quale il demone legge i messaggi
 */
void core(int msqid_param, FILE *daemon_internal_log_fd);

// Message
// Memoria occupata dai due tipi di messaggio. Non bisogna conteggiare il campo type nella quantità di memoria del messaggio
#define COMMAND_SIZE sizeof(Command) // Messaggio stat_msg
#define PROC_SIZE 0                     // Messaggio proc_msg

/**
 * Permessi di default, lettura e scrittura al solo utente
 */
#define USER_PERMS 0600
#define USER_AND_DAEMON_PERMS 0744
#define UMASK_PERMS 027

/**
 * Message types
 */
#define TYPE_STAT 1         // Messaggio di statistiche
#define TYPE_PROC_INIT 2  // Messaggio di inizio processo
#define TYPE_PROC_CLOSE 3 // Messaggio di fine processo

/**
 * Statistics messages struct
 */
typedef struct stat_msg
{
    long type;   // > 0
    Command cmd; //payload, sara' struttura del subcommandresult
} stat_msg;

/**
 * Processes messages struct
 */
typedef struct proc_msg
{
    long type;
} proc_msg;

// API
/**
* Create daemon if needed and establish a connection with it<br>
* Should be done at the beginning of execution
* @return Message queue ID, new or yet present
*/
int createOrGetDaemon();

/**
 * Send a message to the daemon
 * @param msqid Message queue ID toward which send
 * @param cmd Pointer to the <b>Command</b> to send
 */
void send_msg(int msqid, Command *cmd);

/**
 * Notify the daemon that current process has finish<br>
 * <b>This message should always be sent, even when fatal errors occured in current process</b>
 * @param msqid Message queue ID toward which send
 */
void send_close(int msqid);

/**
 * Create a fully working daemon without ruin current execution
 * @param msqid Message queue ID to use to communicate with
 */
void daemonize(int msqid);

/**
 * If an internal error has been encountere, it notifies <br>
 * the main program and prints the error in its internal log
 * @param error_msg error identifier
 * @param secondary_msg second part of the error message
 * @param daemon_internal_log_fd file descriptor for the internal log file
 * @param pid_main process id for the main program
 */
void manageDaemonError(char const *error_msg, char const *secondary_msg, FILE *daemon_internal_log_fd, pid_t pid_main);

/**
 * Logs less important errors than manageDaemonError(...)
 * @param error_msg error message to be printed
 * @param daemon_internal_log_fd file descriptor for the internal log file
 */
void daemonLog(char const *error_msg, FILE *daemon_internal_log_fd);

#endif
