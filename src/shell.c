#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include "shell.h"

char *pwd = NULL;
char *oldpwd = NULL; //initialize the environment variables

void run_shell(run_mode_struct_t run_mode)
{
    config_signals_handlers();
    pwd = getenv("PWD");
    oldpwd = getenv("OLDPWD");
    cmd_t cmd;          // command to be executed
    char line[MAX_LINE_COMMAND];    // command line
    char *args[MAX_LINE_COMMAND / 2 + 1];    // arguments (0 is the command) //should be a pointer because strtok returns a pointer

    FILE *input = stdin;
    if (run_mode.mode == BATCH_MODE)
    {
        input = fopen(run_mode.argument, "r");
        if (input == NULL)
        {
            perror("Error opening batch file");
            exit(EXIT_FAILURE);
        }
    }

    while (TRUE)
    {
        if (run_mode.mode == INTERACTIVE_MODE)
        {
            printf("%s%s$", HOST_NAME, pwd);
            if (!fgets(line, MAX_LINE_COMMAND, input)) break;
        }
        else if (run_mode.mode == BATCH_MODE)
        {
            if (!fgets(line, MAX_LINE_COMMAND, input)) break;
        }

        line[strcspn(line, "\n")] = 0; // remove newline character

        int index_args = 0;
        args[index_args] = strtok(line, " "); // get the first argument

        while (args[index_args] != NULL)
        {
            index_args++;
            args[index_args] = strtok(NULL, " "); // get the next argument
        }

        if (args[0] != NULL)   // args[0] is the command
        {
            cmd = get_cmd(args);
            run_cmd(cmd, args);
        }
    }

    if (run_mode.mode == BATCH_MODE)
    {
        fclose(input);
    }
}

cmd_t get_cmd(char **args)
{
    if (strcmp(args[0], "quit") == 0) // strcmp returns 0 if the strings are equal
        return QUIT;
    else if (strcmp(args[0], "cd") == 0)
        return CD;
    else if (strcmp(args[0], "pwd") == 0)
        return PWD;
    else if (strcmp(args[0], "echo") == 0)
        return ECHO;
    else if (strcmp(args[0], "clr") == 0)
        return CLR;
    else
        return EXTERNAL;    // external program
}

void execute_command(char **args, int input_fd, int output_fd, int is_background)
{
    pid_t pid = fork();
    if (pid == 0)  // child process
    {
        if (input_fd != STDIN_FILENO)
        {
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        if (output_fd != STDOUT_FILENO)
        {
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }
        if (execvp(args[0], args) == -1)
        {
            perror("Error");
            exit(EXIT_FAILURE);
        }
    }
    else if (pid < 0)
    {
        perror("Error");
    }
    else
    {
        if (!is_background)
        {
            foreground_pid = pid; // Set the foreground process PID
            waitpid(pid, NULL, 0); // Wait for the child process to finish if not running in background
            foreground_pid = -1; // Reset the foreground process PID
        }
        else
        {
            printf("Process running in background with PID %d\n", pid);
            printf("\n"); // Print a new line to ensure the prompt appears correctly
        }
    }
}

void run_cmd(cmd_t cmd, char **args)
{
    bg_mode_t background = check_bg(args);

    if (cmd != EXTERNAL)
    {
        switch (cmd)
        {
        case QUIT:
            exit(EXIT_SUCCESS);
            break;
        case CD:
            printf("Changing Dir\n");
            if (args[1] == NULL)
            {
                printf("No directory specified\n");
            }
            else if (strcmp(args[1], "-") == 0)
            {
                if (oldpwd == NULL)
                {
                    printf("No previous directory\n");
                }
                else
                {
                    char *temp = pwd;
                    chdir(oldpwd);
                    pwd = oldpwd;
                    oldpwd = temp;
                    setenv("OLDPWD", pwd, 1);
                    setenv("PWD", oldpwd, 1);
                }
            }
            else if (chdir(args[1]) == -1)
            {
                printf("Directory not found\n");
            }
            else
            {
                chdir(args[1]);
                oldpwd = pwd;
                pwd = getcwd(NULL, 0);
                if (pwd == NULL)
                {
                    perror("getcwd");
                }
                else
                {
                    setenv("OLDPWD", oldpwd, 1);
                    setenv("PWD", pwd, 1);
                }
            }
            break;
        case PWD:
            getcwd(pwd, MAX_LINE_COMMAND);
            printf("%s%s%s", "You are in ", pwd, "\n");
            break;
        case ECHO:
            printf("Echoing\n");
            if (args[1] == NULL)
            {
                printf("No arguments\n");
            }
            else
            {
                for (int i = 1; args[i] != NULL; i++)
                {
                    printf("%s%s", args[i], " ");
                }
                printf("\n");
            }
            break;
        case CLR:
            printf("Clearing\n");
            printf("\033[H\033[J");
            break;
        default:
            break;
        }
    }
    else
    {
        // Handle pipes and redirection
        int pipe_fds[2];
        int input_fd = STDIN_FILENO;
        int output_fd = STDOUT_FILENO;
        int i = 0;
        char *command[MAX_LINE_COMMAND / 2 + 1];
        int is_background = (background == BACKGROUND_MODE);

        while (args[i] != NULL)
        {
            int j = 0;
            while (args[i] != NULL && strcmp(args[i], "|") != 0 && strcmp(args[i], ">") != 0 && strcmp(args[i], "<") != 0)
            {
                command[j++] = args[i++];
            }
            command[j] = NULL;

            if (args[i] != NULL && strcmp(args[i], "|") == 0)
            {
                pipe(pipe_fds);
                execute_command(command, input_fd, pipe_fds[1], is_background);
                close(pipe_fds[1]);
                input_fd = pipe_fds[0];
                i++;
            }
            else if (args[i] != NULL && strcmp(args[i], ">") == 0)
            {
                output_fd = open(args[++i], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (output_fd == -1)
                {
                    perror("Error opening file for redirection");
                    return;
                }
                else {
                  execute_command(command, input_fd, output_fd, is_background);
                }
                i++;
            }
            else if (args[i] != NULL && strcmp(args[i], "<") == 0)
            {
                input_fd = open(args[++i], O_RDONLY);
                if (input_fd == -1)
                {
                    perror("Error opening file for redirection");
                    return;
                }
                else {
                  execute_command(command, input_fd, output_fd, is_background);
                }
                i++;
            }
            else
            {
                execute_command(command, input_fd, output_fd, is_background);
                break;
            }
        }
    }
}