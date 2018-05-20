#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include "../lib/errors.h"
#include "../lib/syscalls.h"
#include "daemon.h"

void daemonize(int msqid)
{

    pid_t cid = w_fork();

    if (cid == 0)
    {
        // Primo figlio
        cid = w_fork();

        // Termino il padre
        if (cid > 0)
        {
            exit(EXIT_SUCCESS);
        }

        // Crea una nuova sessione e un gruppo di processo
        if (setsid() < 0)
        {
            error_fatal(ERR_X, "New session and new process group failed\n");
        }

        // Gestisco o ignoro segnali
        //signal(SIGHUP, SIG_IGN);  //TODO verificare necessità

        // Cambio directory
        if (chdir("/") < 0)
        {
            error_fatal(ERR_X, "Error in changing directory\n");
        }

        // Chiudo tutti i fd
        int x;
        for (x = 0; x < sysconf(_SC_OPEN_MAX); x++)
        {
            close(x);
        }

        core(msqid);
    }

    // Parent esce normalmente
}
