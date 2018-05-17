#include "../lib/errors.h"
#include "../lib/syscalls.h"
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

#include "daemon.h"

#define PIDLENGTH 5
#define PIDPATH "/tmp/xlog/pidpath"

extern int errno;

/**
 * Crea un demone continuando l'esecuzione fuori dalla funzione del processo padre (doppio fork)
 * @param msqid ID della coda di messaggi utilizzata dal demone nella funzione core
 */
void daemonize(int msqid)
{

    pid_t fid = frk();

    if (fid == 0)
    {
        // Primo figlio

        fid = frk();

        // Termino il padre
        if (fid > 0)
        {
            exit(EXIT_SUCCESS);
        }

        // Crea una nuova sessione e un gruppo di processo
        if (setsid() < 0)
        {
            error_fatal(ERR_X, "New session and new process group failed\n");
        }

        // Gestisco o ignoro segnali
        signal(SIGHUP, SIG_IGN);
        signal(SIGCHLD, SIG_IGN);

        // Cambio directory
        if (chdir("/") < 0)
        {
            error_fatal(ERR_X, "Error in changing directory\n");
        }

        // Chiudo tutti i fd
        for (int x = 0; x < sysconf(_SC_OPEN_MAX); x++)
        {
            close(x);
        }

        core(msqid);
    }

    // Parent esce normalmente
}