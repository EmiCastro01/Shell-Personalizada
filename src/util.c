#include <stdio.h>
#include <string.h>
#include "shell.h"

#define FALSE 0
#define TRUE 1
run_mode_struct_t check_mode(int argc, char *argv[])
{
    run_mode_struct_t run_mode;
    if (argc == 1)
    {
        run_mode.mode = INTERACTIVE_MODE;
        run_mode.argument = NULL;
    }
    else
    {
        printf("Batch mode\n");
        run_mode.mode = BATCH_MODE;
        run_mode.argument = argv[1];
    }

    return run_mode;
}

bg_mode_t check_bg(char **args)
{
  bg_mode_t background = MAIN_MODE;
  int arg_index = 0;
  while(args[arg_index] != NULL)
  {
    int len = strlen(args[arg_index]);
    if (len > 0 && args[arg_index][len - 1] == '&')
    {
      background = BACKGROUND_MODE;
      if (len == 1)
      {
        args[arg_index] = NULL; // If the argument is just "&", remove it
      }
      else
      {
        args[arg_index][len - 1] = '\0'; // Remove the "&" from the end of the argument
      }
    }
    arg_index++;
  }
  return background;
}