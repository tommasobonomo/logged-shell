#include "wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

pid_t frk() {
  pid_t fid = fork();
  if (fid < 0) {
    perror("Error during forking process:");
    exit(EXIT_FAILURE);
  }
  return fid;
}