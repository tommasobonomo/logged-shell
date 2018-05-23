#ifndef DAEMON_H
#define DAEMON_H

#include "../lib/commands.h"

#define MSGQUE_PATH "."										// Path per la msg_queue, impostato di default alla directory corrente
#define MSGQUE_NUM 1										// ID numerico della msg_queue, impostato di default a 1
#define DEFAULT_LOGPATH_TXT "/tmp/logged_shell/default.txt" // Path del file di log. TODO: aggiungere l'opzione per customizzarlo
#define DEFAULT_LOGPATH_CSV "/tmp/logged_shell/default.csv" // Path del file di log. TODO: aggiungere l'opzione per customizzarlo
#define DAEMON_ERRORFILE "/tmp/logged_shell/daemon_errors.log"

extern int msqid; // ID msg_queue creata ed attiva

// Core.c
/**
 * Esegue la logica del demone sulla msg_queue msqid_param: legge messaggi passati, decidendo anche quando terminare
 * @param msqid_param la msg_queue sulla dalla quale il demone legge i messaggi
 */
void core(int msqid_param);

// Message
// Memoria occupata dai due tipi di messaggio. Non bisogna conteggiare il campo type nella quantità di memoria del messaggio
#define COMMAND_SIZE sizeof(Command) // Messaggio stat_msg
#define PROCSZ 0					 // Messaggio proc_msg

// Permessi di default, lettura e scrittura al solo utente. TODO: Sono i permessi giusti?
#define PERMS 0600

// Tipologie di messaggio
#define STAT 1		 // Messaggio di statistiche
#define PROC_INIT 2  // Messaggio di inizio processo
#define PROC_CLOSE 3 // Messaggio di fine processo

// Strutture dei messaggi
typedef struct stat_msg
{
	long type;   // > 0
	Command cmd; //payload, sara' struttura del subcommandresult
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
void send_msg(int msqid, Command *cmd);

/**
 * Manda un messaggio proc_msg di tipo PROC_CLOSE per segnalare la chiusura di un processo. Da mandare sempre, anche in caso di terminazione forzata!
 * TODO: Implementare un timeout per evitare demoni infiniti?
 * @param msqid la msg_queue di riferimento per il processo e il demone
 */
void send_close(int msqid);

/**
 * Crea un demone continuando l'esecuzione fuori dalla funzione del processo padre (doppio fork).
 * Praticamente, se un processo chiama questa funzione, verra' creato un demone e il processo continuera' la sua esecuzione
 * @param msqid ID della coda di messaggi utilizzata dal demone nella funzione core
 */
void daemonize(int msqid);

#endif
