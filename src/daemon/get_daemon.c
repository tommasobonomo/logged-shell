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

/*
 * Cerca di recuperare il pid del demone se esiste
 * @return 0 se non esiste il demone, il pid del demone se esiste
 */
int recover_pid() {
  int pid;
  int fd = open(PIDPATH, O_RDONLY);
  if (fd >= 0) {
    char buf[PIDLENGTH];
    read(fd, buf, PIDLENGTH);
    pid = atoi(buf);
  } else {
    pid = 0;
  }
  close(fd);
  return pid;
}

/*
 * Crea un demone eseguendo il kill dei processi che lo generano
 * Scrive il PID del demone nel file specificato nella macro PIDPATH
 */
void daemonize() {

  pid_t fid = frk();

  // Termino il padre
  if (fid > 0) {
    exit(EXIT_SUCCESS);
  }

  // Crea una nuova sessione e un gruppo di processo
  if (setsid() < 0) {
    perror("New session and new process group failed");
    exit(EXIT_FAILURE);
  }

  // Gestisco o ignoro segnali
  signal(SIGHUP, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);

  // Secondo fork
  fid = frk();

  // Scrivo il pid da padre
  if (fid > 0) {
    int fd = open(PIDPATH, O_WRONLY | O_CREAT, 0666);
    if (fd < 0) {
      perror("Error in creating file");
    }

    char str[PIDLENGTH + 1];
    sprintf(str, "%d", fid);
    write(fd, str, PIDLENGTH + 1);
    close(fd);
    exit(EXIT_SUCCESS);
  } else {

    // Cambio directory
    if (chdir("/") < 0) {
      perror("Changing directory failure");
      exit(EXIT_FAILURE);
    }

    // Chiudo tutti i fd
    for (int x = 0; x < sysconf(_SC_OPEN_MAX); x++) {
      close(x);
    }
  }
}

/*
 * Cerca di recuperare pid del demone se esiste:
 * Se esiste, ritorna il pid al chiamante
 * Se non esiste, fa partire il demone e ritorna il suo pid
 */
int main() {

  // Attempt recovery of pid
  int pid = recover_pid();

  // There is no daemon running, create it and execute daemon logic
  if (pid == 0) {
    daemonize();
    // TODO: execvp() della logica del demone
  }

  return 0;
}
