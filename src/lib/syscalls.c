#include "syscalls.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

pid_t frk()
{
  pid_t fid = fork();
  if (fid < 0)
  {
    error_fatal(ERR_X, "Error during forking procedure\n");
  }
  return fid;
}