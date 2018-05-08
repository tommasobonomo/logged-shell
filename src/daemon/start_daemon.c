#include "../lib/wrapper.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PIDLENGTH 5
#define PIDPATH "/tmp/xlog/pidpath"
#define MAXLEN 80

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

  // Termino il padre e scrivo il pid
  if (fid > 0) {
    int fd = open(PIDPATH, O_WRONLY | O_CREAT, 0777);
    if (fd < 0) {
      perror("Error in creating file: ");
    }

    char str[PIDLENGTH + 1];
    sprintf(str, "%d", fid);
    write(fd, str, PIDLENGTH + 1);
    close(fd);
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
    sleep(60);

    break;
  }

  return 0;
}
