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
pid_t monitor_pid = -1;
void run_shell(run_mode_struct_t run_mode, config_t configurations)
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
            run_cmd(cmd, args, configurations);
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
    else if (strcmp(args[0], "start_monitor") == 0)
        return START_MONITOR;
    else if (strcmp(args[0], "stop_monitor") == 0)
        return STOP_MONITOR;
    else if (strcmp(args[0], "status_monitor") == 0)
        return STATUS_MONITOR;
    else if (strcmp(args[0], "show_cpu_usage") == 0)
        return SHOW_CPU_USAGE;
    else if (strcmp(args[0], "show_mem_usage") == 0)
        return SHOW_MEM_USAGE;
    else if (strcmp(args[0], "show_disk_usage") == 0)
        return SHOW_DISK_USAGE;
    else if (strcmp(args[0], "show_net_usage") == 0)
        return SHOW_NET_USAGE;
    else if (strcmp(args[0], "show_proc_no") == 0)
        return SHOW_PROC_NO;
    else
        return EXTERNAL;    // external program
}

void execute_process(char **args, int input_fd, int output_fd, bg_mode_t bg_mode)
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
            printf(getenv("PATH"));
            exit(EXIT_FAILURE);
        }
    }
    else if (pid < 0)
    {
        perror("Error");
    }
    else
    {
        if (bg_mode == MAIN_MODE)
        {
            foreground_pid = pid; // Set the foreground process PID
            waitpid(pid, NULL, 0); // Wait for the child process to finish if not running in background
            foreground_pid = -1; 
        }
        else
        {
            printf("Process running in background with PID %d\n", pid);
            if(args[0] == MONITOR_PATH) 
            monitor_pid = pid;
            printf("\n"); 
        }
    }
}

void run_cmd(cmd_t cmd, char **args, config_t configurations)
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
         case START_MONITOR:
            printf("Starting monitor...\n");
            args[0] = MONITOR_PATH;
            char sampling_interval_str[10];
            snprintf(sampling_interval_str, sizeof(sampling_interval_str), "%d", configurations.sampling_interval);
            args[1] = sampling_interval_str;

            // Añadir métricas como argumentos
            int i;
            for (i = 0; i < configurations.metrics_count; i++) {
                args[i + 2] = configurations.metrics[i];
            }
            args[i + 2] = NULL; // Asegurarse de que la lista de argumentos termine con NULL
            execute_process(args, STDIN_FILENO, STDOUT_FILENO, BACKGROUND_MODE);
            break;
        case STOP_MONITOR:
            printf("Stopping monitor...\n");
            kill(monitor_pid, SIGINT);
            monitor_pid = -1;
            break;
        case STATUS_MONITOR:
            printf("Checking monitor status...\n");
            check_and_print_monitor_status(monitor_pid);
            break;
        case SHOW_CPU_USAGE:
           printf("config json updated.\n");
            char *new_cpu_metric = "cpu_usage";
            configurations.metrics[configurations.metrics_count] = new_cpu_metric;
            update_config_json("../config.json", configurations.metrics, configurations.metrics_count + 1);
            break;
        case SHOW_MEM_USAGE:
            printf("config json updated.\n");
            char *new_mem_metric = "memory_usage";
            configurations.metrics[configurations.metrics_count] = new_mem_metric;
            update_config_json("../config.json", configurations.metrics, configurations.metrics_count + 1);
            break;
        case SHOW_DISK_USAGE:
           printf("config json updated.\n");
            char *new_IO_metric = "IO_time";
            configurations.metrics[configurations.metrics_count] = new_IO_metric;
            update_config_json("../config.json", configurations.metrics, configurations.metrics_count + 1);
            break;  
        case SHOW_NET_USAGE:
            printf("config json updated.\n");
            char *new_tx_metric = "net_TX";
            configurations.metrics[configurations.metrics_count] = new_tx_metric;
            update_config_json("../config.json", configurations.metrics, configurations.metrics_count + 1);
            break;
        case SHOW_PROC_NO:
            printf("config json updated.\n");
            char *new_proc_metric = "processes";
            configurations.metrics[configurations.metrics_count] = new_proc_metric;
            update_config_json("../config.json", configurations.metrics, configurations.metrics_count + 1);
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
        bg_mode_t bg_mode = BACKGROUND_MODE;

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
                execute_process(command, input_fd, pipe_fds[1], bg_mode);
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
                  execute_process(command, input_fd, output_fd, bg_mode);
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
                  execute_process(command, input_fd, output_fd, bg_mode);
                }
                i++;
            }
            else
            {
                execute_process(command, input_fd, output_fd, bg_mode);
                break;
            }
        }
    }
}