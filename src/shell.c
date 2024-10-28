#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"


void run_shell()
{
  pwd = getenv("PWD"), oldpwd = getenv("OLDPWD"); //initialize the environment variables
  cmd_t cmd;          // command to be executed
  char line[MAX_LINE_COMMAND];    // command line
  char *args[MAX_LINE_COMMAND];    // arguments (0 is the command) //should be a pointer because strtok returns a pointer
  
  while(TRUE)
    {
      printf(HOST_NAME);
      if(!fgets(line, MAX_LINE_COMMAND, stdin)) break;
      line[strcspn(line, "\n")] = 0; // remove enter
      
      int index_args = 0;
      args[index_args] = strtok(line, " "); // get the first argument
      while(args[index_args] != NULL){
        index_args++;
        args[index_args] = strtok(NULL, " "); // get the next argument
      }

      if(args[0] != NULL)   // args[0] is the command
        cmd = get_cmd(args);
      
      run_cmd(cmd, args);
    }

}

cmd_t get_cmd(char **args)
{
  if(strcmp(args[0], "quit") == 0) // strcmp returns 0 if the strings are equal
    return QUIT;
  else if(strcmp(args[0], "cd") == 0)
    return CD;
  else if(strcmp(args[0], "pwd") == 0)
    return PWD;
  else if(strcmp(args[0], "echo") == 0)
    return ECHO;
  else if(strcmp(args[0], "clr") == 0)
    return CLR;
    else
    {
      printf("Command not found\n");
      return NOP;
    }
    
}

void run_cmd(cmd_t cmd, char **args)
{
  switch (cmd)
  {
  case QUIT:
    exit(EXIT_SUCCESS);
    break;
  case CD:
    printf("Changin Dir\n");
    // usar chdir
    

    break;
  case PWD:
    printf("Print Working dir\n");
      
    break;
  case ECHO:
    printf("EChoing\n");
    break;
  case CLR:
    printf("Clearing\n");
    break;
  default:
  // NOP cases
    break;
  }
}