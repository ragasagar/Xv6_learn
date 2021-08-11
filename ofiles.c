#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[])
{	
  int nfiles = numOpenFiles();
  int expected = 3;
  if(nfiles != expected)
      printf(1,"Test Case Failed found %d file opened Expected %d\n", nfiles,expected);
  else
  	printf(1,"Success Test 1\n");
  //open
  int fd1  = open("test1" ,O_WRONLY|O_CREATE);
  if(fd1 > 0)
   expected++;
  nfiles = numOpenFiles();
  
  if(nfiles != expected)
      printf(1,"Test Case Failed found %d file opened Expected %d\n", nfiles,expected);
   else
  	printf(1,"Success Test 2\n");
  
  int fd2  = open("test2" ,O_WRONLY|O_CREATE);
  if(fd2 > 0)
   expected++;
  nfiles = numOpenFiles();
  
  if(nfiles != expected)
      printf(1,"Test Case Failed found %d file opened Expected %d\n", nfiles,expected);
  else
      printf(1,"Success Test 3\n");
  
  int fd3  = open("test3" ,O_WRONLY|O_CREATE);
  if(fd3 > 0)
   expected++;
  nfiles = numOpenFiles();
  
  if(nfiles != expected)
      printf(1,"Test Case Failed found %d file opened Expected %d\n", nfiles,expected);
  else
     printf(1,"Success Test 4\n");
  
  int fd4  = open("test4" ,O_RDONLY|O_CREATE);
  if(fd4 > 0)
   expected++;
  nfiles = numOpenFiles();
  
  if(nfiles != expected)
      printf(1,"Test Case Failed found %d file opened Expected %d\n", nfiles,expected);
  else
      printf(1,"Success Test 5\n");
  
  int fd5  = open("test5" ,O_RDONLY|O_CREATE);
  if(fd5 > 0)
   expected++;
  nfiles = numOpenFiles();
  
  if(nfiles != expected)
      printf(1,"Test Case Failed found %d file opened Expected %d\n", nfiles,expected);
  else
     printf(1,"Success Test 6\n");
  
  int fd6  = open("test6" ,O_RDONLY|O_CREATE);
  if(fd6 > 0)
    expected++;
  nfiles = numOpenFiles();
  if(nfiles != expected)
    printf(1,"Test Case Failed found %d file opened Expected %d\n", nfiles,expected);
  else
     printf(1,"Success Test 7\n");
  if(fd1 >0)
   close(fd1);
  
  if(fd2 >0)
   close(fd1);
  
  if(fd3 >0)
   close(fd1);
  
  if(fd4 >0)
   close(fd1);
  
  if(fd5 >0)
   close(fd1);
  
  if(fd6 >0)
   close(fd1);
 exit(0);
}