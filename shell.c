// Custom shellfor assignment.

#include "types.h"
#include "user.h"
#include "fcntl.h"


#define MAXARGS 10
char *validCommand[] = {"ls", "cat", "grep", "echo", "wc", "ps", "procinfo", "rm" , "hw", "ofiles", 
"memalloc","processtime", "mkdir", "kill", "ln"};
char *testCommands[10] = {
  "\ncat README", "\n cat README > hello.txt" "\n"
};


//these symbols  will be considered as white spaces.
char whitespace[] = " \t\r\n\v";

//total symbol which decides the type of te command.
char symbols[] = "<|>&;()";


// general exec command to store the exec type command.
struct execcommand {
  int type;
  char *argv[MAXARGS];
  char *temp[MAXARGS];
};

// general structure to define pipe, io command
struct command{
  int type;
  struct execcommand *child1;
  struct execcommand *child2;
  char *file;
  char *tempfile;
  int mode;
  int fd;
};

struct execcommand *parsecommands(char*);
int parsefileexecifexist(char *);
int strcmpprefix(char *, char *);
int createtestcasefile(void);


//This method checks the valid commands to run the program.
int
isValid(char *arg){
  char **p;
  p = validCommand;
  while(*p != 0){
    if(strcmp(arg, *p)==0)
      return 1;
    ++p;
  }
   return 0;
}

// execute command
void
runcmd(struct execcommand *cmd)
{
  int pipefd[2];
  struct execcommand *execcommand;
  struct command *command;
  int status;

  if(cmd == 0)
    exit(0);

  switch(cmd->type){
  case 1:
    execcommand = (struct execcommand*) cmd;
    if(execcommand->argv[0] == 0)
      break;
    if(isValid(execcommand->argv[0])){
      exec(execcommand->argv[0], execcommand->argv);
      printf(2, "exec %s failed\n", execcommand->argv[0]);
    }else
      printf(1, "Illegal command or arguments\n");
    break;


  case 2:
    command = (struct command*)cmd;
    close(command->fd);
    if(open(command->file, command->mode) < 0){
      printf(2, "open %s failed\n", command->file);
      exit(0);
    }
    runcmd(command->child1);
    break;

  case 4:
    command = (struct command*)cmd;
    if(fork() == 0)
      runcmd(command->child1);
    wait(0);
    runcmd(command->child2);
    break;

  case 3:
    if(pipe(pipefd) < 0)
      exit(0);
    command = (struct command*)cmd;
    if(fork() == 0){
      close(1);
      dup(pipefd[1]);
      close(pipefd[0]);
      close(pipefd[1]);
      runcmd(command->child1);
    }
    if(fork() == 0){
      close(0);
      dup(pipefd[0]);
      close(pipefd[0]);
      close(pipefd[1]);
      runcmd(command->child2);
    }
    close(pipefd[0]);
    close(pipefd[1]);
    wait(0);
    wait(0);
    break;

    case 5:
    command = (struct command*) cmd;
      if(fork()==0){
        runcmd(command->child1);
        sleep(20);
      }
      wait(&status);
      if(status == 0){
        if(fork()==0){
          runcmd(command->child2);
        }
        wait(0);
      }
      break;
    case 6:
    command = (struct command*) cmd;
      if(fork()==0){
        runcmd(command->child1);
      }
      wait(&status);
      if(status == -1){
        if(fork()==0){
          runcmd(command->child2);
        }
        wait(0);
      }
      break;
  }
  exit(0);
}

int
getcmd(char *buf, int nbuf)
{
  printf(2, "MyShell> ");
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  if(buf[0] == 0)
    return -1;
  return 0;
}

int
main(void)
{
  static char buf[100];

  //create the input test file for question no: 7
  createtestcasefile();

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){

    //if no command is given just continue the shell.
    if(strlen(buf)==1 && strcmp(buf,"\n")==0)
      continue;
    
    if(strcmpprefix(buf, "executeCommands")==0){
      parsefileexecifexist(buf);
      continue;
    }
    //if exit command is given exit the current shell.
    if(buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'i' && buf[3] == 't'){ 
      break;
    }

    //rest commands are parsed and executed using exec call.
    if(fork() == 0){
      runcmd(parsecommands(buf));
    }
    wait(0);
  }
  exit(0);
}


struct execcommand *fetchexeccommand(char**, char*);
struct execcommand *cleanvalues(struct execcommand*);
struct execcommand *buildcommand(struct execcommand *, char **, char *, int tok);

/**
 * 
 *  In this program, it uses three types of commands, normal exec, io and pipe. basically the basic atomic 
 * command.
*/
struct execcommand*
execcmd(void)
{
  struct execcommand *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = 1;
  return cmd;
}

//This holds the information for redirection type of commands.
struct execcommand*
iocommand(struct execcommand *child, char *mainfile, char *tempfile, int mode, int fd)
{
  struct command *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = 2;
  cmd->child1 = child;
  cmd->file = mainfile;
  cmd->tempfile = tempfile;
  cmd->mode = mode;
  cmd->fd = fd;
  return (struct execcommand*)cmd;
}


/**
 * this holds the information regarding pipe/parallel/&&/|| type of the command.
 * type is used to differentiate the type of command.
 * */
struct execcommand*
command(struct execcommand *child1, struct execcommand *child2, int type)
{
  struct command *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = type;
  cmd->child1 = child1;
  cmd->child2 = child2;
  return (struct execcommand*)cmd;
}


/**
 * Checks if the next char is given toks or not.
 **/
int
nextchar(char **ps, char *es, char *toks)
{
  char *s;

  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return *s && strchr(toks, *s);
}

/**
 * Main parser which parse the commands.
 * 
 * */
struct execcommand*
parsecommands(char *s){
  char *end;
  struct execcommand *cmd;
  end = s + strlen(s);
  int tok;

  //first we try to fetch the first exec command
  cmd = fetchexeccommand(&s, end);

  while(s<end && s !=0){
    
    tok = *s;
    s++;
    switch(tok){
      // this represent pipe command.
      case '|':
        //this represent the or command.
        if(nextchar(&s, end, "|")){
          s++;
          cmd = command(cmd, fetchexeccommand(&s, end),6);
        }else{
          cmd = command(cmd, fetchexeccommand(&s, end),3);
        }
        break;
      
      // this represent the parallel command.
      case ';':
        cmd = command(cmd, fetchexeccommand(&s, end), 4);
        break;

      //this represent and commands
      case '&':
        if(nextchar(&s, end, "&")){
          s++;
          cmd = command(cmd, fetchexeccommand(&s, end),5);
        }
        break;
    
      //this represent the i/o direction command.
      case '<':
      case '>':
        cmd = buildcommand(cmd, &s, end, tok);
        break;
    }
  }

//cleaning the values which has more values than the command
  if(s==end){
    cleanvalues(cmd);
  }
  return cmd;
}



struct execcommand*
buildcommand(struct execcommand *cmd, char **s, char *end, int tok){
  char *temp1, *temp2, *val;
  val= *s;

  // this will run until it cover the redirection code.
  if(tok == '<' || tok =='>'){
    //clearing the whitespace unitil it reaches the command.
    while(val<end && strchr(whitespace, *val)){
      val++;
    }
    temp1=val;


    //fetching the first command util the another space or symbol come.
    while(val<end && !strchr(whitespace, *val) && !strchr(symbols, *val))
      val++;

    // copy the rest part of the command which may be input file or output file.
    temp2 = val;

    // remove the whitespaces
    while(val<end && strchr(whitespace, *val))
    val++;

    //depends on the token that was fetched, we need to decide its read or write.
    switch(tok){

      //represents the input file
      case '<':
      cmd = iocommand(cmd, temp1, temp2, O_RDONLY, 0);
      break;

    //represents the output file
    case '>':
      cmd = iocommand(cmd, temp1, temp2, O_WRONLY|O_CREATE, 1);
      break;
    }

    // copy the rest command to original string.
    *s = val;
  }
  return cmd;
}

struct execcommand* 
fetchexeccommand(char **s, char *end){
  char *val, *tempval, *tempval2;

  struct execcommand  *cmd;

  cmd = execcmd();

  val = *s;
  int count = 0;
  while(val <= end && !nextchar(&val, end, "<>|&;")){
  
  //removing first white space before the statement
  while(val<end && strchr(whitespace, *val)){
    val++;
  }
  //storing the commands values inside the temp value.
  tempval = val;


  //iterating over the command until we receive the whitespace or any other special symbol.
  while(val < end && !strchr(whitespace, *val) && !strchr(symbols, *val))
    val++;

  //storing the string after certian command is extracted from the string.
  tempval2 = val;

  //removing the whitespace till another command.
  while(val<end && strchr(whitespace, *val))
  val++;

  //stroring the first command extracted inside the first array.
  cmd->argv[count]=tempval;

  //store the later part of the string to another string.
  cmd->temp[count++]=tempval2;
  
  *s = val;

  if(*val == 0){
    break;
  }
  }
  //return the remaing string to the original string.
  cmd->argv[count]=0;
  cmd->temp[count]=0;

  return cmd;

}


//this metod is required to clean up the unwanted 
// command/arguement of the given command string
struct execcommand*
cleanvalues(struct execcommand *cmd)
{
  int i;
  struct command *command;

  if(cmd == 0)
    return 0;

  switch(cmd->type){
  case 1:
    for(i=0; cmd->argv[i]; i++)
      *cmd->temp[i] = 0;
    break;

  case 2:
    command = (struct command*)cmd;
    cleanvalues(command->child1);
    *command->tempfile = 0;
    break;

  case 3:
  case 4:
  case 5:
  case 6:
    command = (struct command*) cmd;
    cleanvalues(command->child1);
    cleanvalues(command->child2);
  }
  return cmd;
}


int
parsefileexecifexist(char *s){
  char *val, *end,cc, buf[512];
  struct execcommand *cmd;
  int fd,n,c,i;
  val=s;
  end = s + strlen(s);

  cmd = fetchexeccommand(&val, end);
  cleanvalues(cmd);
  if((fd = open(cmd->argv[1], 0)) < 0){
      printf(1, "executeCommands: cannot open %s\n", cmd->argv[1]);
      return 0;
    }
  
  c=0;
  for(i=0; i+1 < sizeof(buf); ){
    if((n= read(fd, &cc, 1))>0){
      if(cc!='\n')
        buf[c++]= cc;
      if(cc == '\n'  && strlen(buf)>0){
        if(fork()==0){
          runcmd(parsecommands(buf));
        }
        wait(0);
        memset(buf, 0, strlen(buf));
        c=0;
      }
    }else{
      if(strlen(buf)>0){
        if(fork()==0){
          runcmd(parsecommands(buf));
        }
        wait(0);
      }
      break;
    }
  }

  printf(1,"\n");
  close(fd);
  return 0;
}

int
strcmpprefix(char *p, char *q){
  char *val, *end;

  val = p;
  end= q+ strlen(q);
  while(strchr(whitespace, *val)){
    val++;
  }

  while(!strchr(whitespace, *val) && !strchr(symbols, *val)){
    if(q<end && *val!=*q )
      return -1;
    val++; q++;
  }

  return 0;
}

int
createtestcasefile(){
  int fd, n;

  if((fd = open("test", O_RDWR | O_CREATE))<0){
    printf(1, "Unable to create test case files");
    return -1;
  }

 char **p;
  p = testCommands;
  while(*p != 0){
    n= strlen(*p);
    if (write(fd, *p, n) != n) 
      return -1;
     ++p;
  }

  close(fd);
  return 0;
}