#include "../lib/wrapper.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void daemonize() {

  pid_t fid = frk();

  // Termino il padre
  if (fid > 0) {
    exit(EXIT_SUCCESS);
  }

  // Crea una nuova sessione e un gruppo di processo
  if (setsid() < 0) {
    perror("New session and new process group failed:");
    exit(EXIT_FAILURE);
  }

  // Gestisco o ignoro segnali
  signal(SIGHUP, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);

  // Secondo fork
  fid = frk();

  // Termino il padre
  if (fid > 0) {
    exit(EXIT_SUCCESS);
  }

  // Cambio directory
  if (chdir("/") < 0) {
    perror("Changing directory failure:");
    exit(EXIT_FAILURE);
  }

  // Chiudo tutti i fd
  for (int x = 0; x < sysconf(_SC_OPEN_MAX); x++) {
    close(x);
  }
}

int main() {
  daemonize();

  while (1) {

    // Daemon executing
    sleep(30);

    break;
  }

  return 0;
}