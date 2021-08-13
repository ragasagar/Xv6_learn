// Shell.

#include "types.h"
#include "user.h"
#include "fcntl.h"


#define MAXARGS 10

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>&;()";

struct cmds{
  int type;
};

// general exec command to store the exec type command.
struct execcommand {
  int type;
  char *argv[MAXARGS];
  char *temp[MAXARGS];
};

// general structure to define pipe, io command
struct command{
  int type;
  struct cmds *child1;
  struct cmds *child2;
  char *file;
  char *tempfile;
  int mode;
  int fd;
};

struct cmds *parsecmd(char*);


//This method checks the valid commands to run the program.
int
isValid(char *arg){
  char *validCommand[] = {"ls", "cat", "grep", "echo", "wc", "ps", "procinfo"};
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
runcmd(struct cmds *cmd)
{
  int p[2];
  struct execcommand *execcommand;
  struct command *command;

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
    if(pipe(p) < 0)
      exit(0);
    command = (struct command*)cmd;
    if(fork() == 0){
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(command->child1);
    }
    if(fork() == 0){
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(command->child2);
    }
    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);
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
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

int
main(void)
{
  static char buf[100];

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){

    //if no command is given just continue the shell.
    if(strlen(buf)==1 && strcmp(buf,"\n")==0)
      continue;

    //if exit command is given exit the current shell.
    if(buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'i' && buf[3] == 't'){ 
      break;
    }

    //rest commands are parsed and executed using exec call.
    if(fork() == 0){
      runcmd(parsecmd(buf));
    }
    wait(0);
  }
  exit(0);
}


struct cmds *fetchexeccommand(char**, char*);
struct cmds *cleanvalues(struct cmds*);
struct cmds *buildcommand(struct cmds *, char **, char *);

/**
 * 
 *  In this program, it uses three types of commands, normal exec, io and pipe.
**/
struct cmds*
execcmd(void)
{
  struct execcommand *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = 1;
  return (struct cmds*)cmd;
}

struct cmds*
iocommand(struct cmds *subcmd, char *file, char *efile, int mode, int fd)
{
  struct command *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = 2;
  cmd->child1 = subcmd;
  cmd->file = file;
  cmd->tempfile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  return (struct cmds*)cmd;
}

struct cmds*
command(struct cmds *left, struct cmds *right, int type)
{
  struct command *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = type;
  cmd->child1 = left;
  cmd->child2 = right;
  return (struct cmds*)cmd;
}


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

struct cmds*
parsecmd(char *s){
  char *end;
  struct cmds *cmd;
  end = s + strlen(s);
  cmd = fetchexeccommand(&s, end);
  if(nextchar(&s, end, "|")){

    s++;
    cmd = command(cmd, fetchexeccommand(&s, end),3);
  }
  else if(nextchar(&s, end, ";")){
    s++;
    cmd = command(cmd, fetchexeccommand(&s, end), 4);
  }
  while(nextchar(&s, end, "<>")){
    cmd = buildcommand(cmd, &s, end);
    if(nextchar(&s, end, "|")){
      s++;
      cmd = command(cmd, fetchexeccommand(&s, end), 3);
    }
  }

  if(s==end){
    cleanvalues(cmd);
  }
  return cmd;
}



struct cmds*
buildcommand(struct cmds *cmd, char **s, char *end){
  int tok;
  char *temp1, *temp2, *val;
  val= *s;
  while(nextchar(&val, end, "<>")){
    tok = *val;
    val++;
    while(val<end && strchr(whitespace, *val)){
      val++;
    }
    temp1=val;
    while(val<end && !strchr(whitespace, *val) && !strchr(symbols, *val))
    val++;
    temp2 = val;
    while(val<end && strchr(whitespace, *val))
    val++;
    switch(tok){
      case '<':
      cmd = iocommand(cmd, temp1, temp2, O_RDONLY, 0);
      break;
    case '>':
      cmd = iocommand(cmd, temp1, temp2, O_WRONLY|O_CREATE, 1);
      break;
    case '+':  // >>
      cmd = iocommand(cmd, temp1, temp2, O_WRONLY|O_CREATE, 1);
      break;
    }

    *s = val;
  }
  return cmd;
}

struct cmds* 
fetchexeccommand(char **s, char *end){
  char *val, *tempval, *tempval2;

  struct execcommand  *cmd;
  struct cmds *ret;

  ret = execcmd();
  cmd = (struct execcommand*)ret;

  val = *s;
  int count = 0;
  while(val <= end && !nextchar(&val, end, "<>|)&;")){
  
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

  return ret;

}


//this metod is required to clean up the unwanted 
// command/arguement of the given command string
struct cmds*
cleanvalues(struct cmds *cmd)
{
  int i;
  struct execcommand *ecmd;
  struct command *pcmd;
  struct command *rcmd;

  if(cmd == 0)
    return 0;

  switch(cmd->type){
  case 1:
    ecmd = (struct execcommand*)cmd;
    for(i=0; ecmd->argv[i]; i++)
      *ecmd->temp[i] = 0;
    break;

  case 2:
    rcmd = (struct command*)cmd;
    cleanvalues(rcmd->child1);
    *rcmd->tempfile = 0;
    break;

  case 3:
  case 4:
    pcmd = (struct command*) cmd;
    cleanvalues(pcmd->child1);
    cleanvalues(pcmd->child2);
  }
  return cmd;
}