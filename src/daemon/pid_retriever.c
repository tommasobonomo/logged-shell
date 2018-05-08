#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PIDPATH "/tmp/xlog/pidpath.txt"
#define MAXLEN 80

int main() {
  int fd = open(PIDPATH, O_RDONLY, 0755);
  if (fd < 0) {
    printf("Error in opening fifo\n");
  } else {
    char str[MAXLEN];
    read(fd, str, MAXLEN);
    printf("Daemon pid: %s\n", str);
  }

  return 0;
}