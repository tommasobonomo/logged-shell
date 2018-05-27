#include "./daemon.h"
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include "../lib/errors.h"
#include "../lib/syscalls.h"

void daemonize(int msqid)
{
    pid_t cid = w_fork();

    if (cid == 0)
    {
        // Crea una nuova sessione e un gruppo di processo
        w_setsid();

        // Cambio directory
        w_chdir("/");

        // Primo figlio
        cid = w_fork();

        // Termino il padre
        if (cid > 0)
        {
            exit(EXIT_SUCCESS);
        }

        // Close all file descriptors
        int x;
        for (x = 0; x < sysconf(_SC_OPEN_MAX); x++)
        {
            close(x);
        }

        /*resettign File Creation Mask */
        umask(UMASK_PERMS);

        //File to store daemon internal logs
        FILE *daemon_internal_log_fd = fopen(DAEMON_INTERNAL_LOGFILE, APPEND);
        if (daemon_internal_log_fd == NULL)
        {
            daemon_internal_log_fd = fopen(DAEMON_INTERNAL_LOGFILE2, APPEND);
            if (daemon_internal_log_fd != NULL)
            {
                manageDaemonError("Can't open daemon error file", DAEMON_INTERNAL_LOGFILE, daemon_internal_log_fd, PID_MAIN_UNKNOWN);
            }
            else
            {
                msgctl(msqid, IPC_RMID, NULL);
                exit(37); //TODO cambia
            }
        }

        daemonLog("DAEMON ON", daemon_internal_log_fd);

        core(msqid, daemon_internal_log_fd);
    } // Parent continue normally
}
