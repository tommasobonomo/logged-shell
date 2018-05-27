#include "./daemon.h"
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
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

		// Chiudo tutti i fd
		int x;
		for (x = 0; x < sysconf(_SC_OPEN_MAX); x++)
		{
			close(x);
		}

		/*resettign File Creation Mask */
		umask(027);

		core(msqid);
	} // Parent esce normalmente
}
