// Shell.

#include "types.h"
#include "user.h"
#include "fcntl.h"

#define MAXARGS 100

struct cmd {
  int type;
  char *cmd[MAXARGS];
};

int
getcmd(char *buf, int nbuf)
{
  printf(2, "MyShell> ");
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

int
main(void)
{
  static char buf[100];
  int fd;

  // Ensure that three file descriptors are open.
  while((fd = open("console", O_RDWR)) >= 0){
    if(fd >= 3){
      close(fd);
      break;
    }
  }

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'i' && buf[3] == 't'){ 
      printf(1,"%s", buf);
      break;
    }
    if(fork() == 0){
      
    }
    wait(0);
  }
  exit(0);
}
