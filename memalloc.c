#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"


int main(int argc, char *argv[])
{
  printf(1,"Memory allocated till now: %d bytes\n", memAlloc());
  sleep(10);

  char* x = sbrk(1024);
  printf(1,"Memory allocated till now: %d bytes\n", memAlloc());
  
  //free(x);
  char* y = sbrk(4096);
  printf(1,"Memory allocated till now: %d bytes\n", memAlloc());


  printf(1,"Memory allocated till now: %d bytes\n", memAlloc());
  

  char* z = sbrk(-10);
  printf(1,"Memory allocated till now: %d bytes\n", memAlloc());

  char* a = sbrk(-20000);
  printf(1,"Memory allocated till now: %d bytes\n", memAlloc());



  free(x);
  free(y);
  free(z);  
  free(a);
  exit();
}