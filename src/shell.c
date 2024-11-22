#include "shell.h"
#include <cjson/cJSON.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Working directory global variable
 */
char* pwd = NULL;
/**
 * Old working directory global variable
 */
char* oldpwd = NULL;
/**
 * monitor pID
 */
pid_t monitor_pid = -1;
/**
 * @brief Executes de shell system on the mode specified
 * and with the configurations passed
 * @param run_mode 
 * @param configurations 
 */
void run_shell(run_mode_struct_t run_mode, config_t configurations)
{
    config_signals_handlers();
    pwd = getenv("PWD");
    oldpwd = getenv("OLDPWD");
    cmd_t cmd;                   // command to be executed
    char line[MAX_LINE_COMMAND]; // command line
    char* args[MAX_LINE_COMMAND / 2 +
               1]; // arguments (0 is the command) //should be a pointer because strtok returns a pointer

    FILE* input = stdin;
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
            if (!fgets(line, MAX_LINE_COMMAND, input))
            {
                break;
            }
        }
        else if (run_mode.mode == BATCH_MODE)
        {
            if (!fgets(line, MAX_LINE_COMMAND, input))
            {
                break;
            }
        }

        line[strcspn(line, "\n")] = 0; // remove newline character

        int index_args = 0;
        args[index_args] = strtok(line, " "); // get the first argument

        while (args[index_args] != NULL)
        {
            index_args++;
            args[index_args] = strtok(NULL, " "); // get the next argument
        }

        if (args[0] != NULL) // args[0] is the command
        {
            cmd = get_cmd(args);
            run_cmd(cmd, args, &configurations);
        }
    }

    if (run_mode.mode == BATCH_MODE)
    {
        fclose(input);
    }
}

/**
 * @brief gets the command to be executed based on the args
 * 
 * @param args 
 * @return cmd_t 
 */
cmd_t get_cmd(char** args)
{
    if (strcmp(args[0], "quit") == 0)
    {
        return QUIT;
    } 
    else if (strcmp(args[0], "cd") == 0)
    {
        return CD;
    }
    else if (strcmp(args[0], "pwd") == 0)
    {
        return PWD;
    }
    else if (strcmp(args[0], "echo") == 0)
    {
        return ECHO;
    }
    else if (strcmp(args[0], "clr") == 0)
    {
        return CLR;
    }
    else if (strcmp(args[0], "start_monitor") == 0)
    {
        return START_MONITOR;
    }
    else if (strcmp(args[0], "stop_monitor") == 0)
    {
        return STOP_MONITOR;
    }
    else if (strcmp(args[0], "status_monitor") == 0)
    {
        return STATUS_MONITOR;
    }
    else if (strcmp(args[0], "show_cpu_usage") == 0)
    {
        return SHOW_CPU_USAGE;
    }
    else if (strcmp(args[0], "show_mem_usage") == 0)
    {
        return SHOW_MEM_USAGE;
    }
    else if (strcmp(args[0], "show_disk_usage") == 0)
    {
        return SHOW_DISK_USAGE;
    }
    else if (strcmp(args[0], "show_net_usage") == 0)
    {
        return SHOW_NET_USAGE;
    }
    else if (strcmp(args[0], "show_proc_no") == 0)
    {
        return SHOW_PROC_NO;
    }
    else
    {
        return EXTERNAL; // external program
    }
}

/**
 * @brief Executes the process based on command passed.
 * 
 * @param args 
 * @param input_fd 
 * @param output_fd 
 * @param bg_mode 
 */
void execute_process(char** args, int input_fd, int output_fd, bg_mode_t bg_mode)
{
    pid_t pid = fork();
    if (pid == 0) // child process
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
            foreground_pid = pid;  // Set the foreground process PID
            waitpid(pid, NULL, 0); // Wait for the child process to finish if not running in background
            foreground_pid = -1;
        }
        else
        {
            printf("Process running in background with PID %d\n", pid);
            if (args[0] == MONITOR_PATH)
            {
                monitor_pid = pid;
            }
            printf("\n");
        }
    }
}

/**
 * @brief Analyze and calls execute_process based on command passed.
 * 
 * @param cmd 
 * @param args 
 * @param configurations 
 * @return int 
 */
int run_cmd(cmd_t cmd, char** args, config_t* configurations)
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
                return -1;
            }
            else if (strcmp(args[1], "-") == 0)
            {
                if (oldpwd == NULL)
                {
                    printf("No previous directory\n");
                    return -1;
                }
                else
                {
                    char* temp = pwd;
                    chdir(oldpwd);
                    pwd = oldpwd;
                    oldpwd = temp;
                    setenv("OLDPWD", pwd, 1);
                    setenv("PWD", oldpwd, 1);
                    return 0;
                }
            }
            else if (chdir(args[1]) == -1)
            {
                printf("Directory not found\n");
                return -1;
            }
            else
            {
                chdir(args[1]);
                oldpwd = pwd;
                pwd = getcwd(NULL, 0);
                if (pwd == NULL)
                {
                    perror("getcwd");
                    return -1;
                }
                else
                {
                    setenv("OLDPWD", oldpwd, 1);
                    setenv("PWD", pwd, 1);
                    return 0;
                }
            }
            break;
        case PWD:
            getcwd(pwd, MAX_LINE_COMMAND);
            printf("%s%s%s", "You are in ", pwd, "\n");
            return 0;
        case ECHO:
            printf("Echoing\n");
            if (args[1] == NULL)
            {
                printf("No arguments\n");
                return -1;
            }
            else
            {
                for (int i = 1; args[i] != NULL; i++)
                {
                    printf("%s%s", args[i], " ");
                }
                printf("\n");
                return 0;
            }
            break;
        case CLR:
            printf("Clearing\n");
            printf("\033[H\033[J");
            return 0;
        case START_MONITOR:
            printf("Starting monitor...\n");
            args[0] = MONITOR_PATH;
            char sampling_interval_str[10];
            snprintf(sampling_interval_str, sizeof(sampling_interval_str), "%d", configurations->sampling_interval);
            args[1] = sampling_interval_str;

            // Añadir métricas como argumentos
            int i;
            for (i = 0; i < configurations->metrics_count; i++)
            {
                args[i + 2] = configurations->metrics[i];
            }
            args[i + 2] = NULL; // Asegurarse de que la lista de argumentos termine con NULL
            execute_process(args, STDIN_FILENO, STDOUT_FILENO, BACKGROUND_MODE);
            return 0;
        case STOP_MONITOR:
            printf("Stopping monitor...\n");
            kill(monitor_pid, SIGINT);
            monitor_pid = -1;
            return 0;
        case STATUS_MONITOR:
            printf("Checking monitor status...\n");
            check_and_print_monitor_status(monitor_pid);
            return 0;
        case SHOW_CPU_USAGE:
            printf("config json updated.\n");
            char* new_cpu_metric = "cpu_usage";
            configurations->metrics[configurations->metrics_count] = new_cpu_metric;
            update_config_json("../config.json", configurations->metrics, configurations->metrics_count + 1);
            return 0;
        case SHOW_MEM_USAGE:
            printf("config json updated.\n");
            char* new_mem_metric = "memory_usage";
            configurations->metrics[configurations->metrics_count] = new_mem_metric;
            update_config_json("../config.json", configurations->metrics, configurations->metrics_count + 1);
            return 0;
        case SHOW_DISK_USAGE:
            printf("config json updated.\n");
            char* new_IO_metric = "IO_time";
            configurations->metrics[configurations->metrics_count] = new_IO_metric;
            update_config_json("../config.json", configurations->metrics, configurations->metrics_count + 1);
            return 0;
        case SHOW_NET_USAGE:
            printf("config json updated.\n");
            char* new_tx_metric = "net_TX";
            configurations->metrics[configurations->metrics_count] = new_tx_metric;
            update_config_json("../config.json", configurations->metrics, configurations->metrics_count + 1);
            return 0;
        case SHOW_PROC_NO:
            printf("config json updated.\n");
            char* new_proc_metric = "proccesses";
            configurations->metrics[configurations->metrics_count] = new_proc_metric;
            update_config_json("../config.json", configurations->metrics, configurations->metrics_count + 1);
            return 0;
        default:
            return -1;
        }
    }
    else
    {
        // Handle pipes and redirection
        int pipe_fds[2];
        int input_fd = STDIN_FILENO;
        int output_fd = STDOUT_FILENO;
        int i = 0;
        char* command[MAX_LINE_COMMAND / 2 + 1];
        bg_mode_t bg_mode = BACKGROUND_MODE;

        while (args[i] != NULL)
        {
            int j = 0;
            while (args[i] != NULL && strcmp(args[i], "|") != 0 && strcmp(args[i], ">") != 0 &&
                   strcmp(args[i], "<") != 0)
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
                    return -1;
                }
                else
                {
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
                    return -1;
                }
                else
                {
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
    return 0;
}
/**
 * @brief loads data from the config.json file
 * and stores it in the configurations struct
 * 
 * @param filename 
 * @param configurations 
 */
void load_config_json(const char* filename, config_t* configurations)
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        perror("Unable to open config file");
        return;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* data = (char*)malloc(length + 1);
    fread(data, 1, length, file);
    fclose(file);
    data[length] = '\0';

    cJSON* json = cJSON_Parse(data);
    if (!json)
    {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        free(data);
        return;
    }

    cJSON* interval = cJSON_GetObjectItem(json, "sampling_interval");
    cJSON* metrics = cJSON_GetObjectItem(json, "metrics");

    if (cJSON_IsNumber(interval))
    {
        printf("Sampling interval: %d\n", interval->valueint);
        configurations->sampling_interval = interval->valueint;
    }

    if (cJSON_IsArray(metrics))
    {
        int size = cJSON_GetArraySize(metrics);
        configurations->metrics_count = size;
        for (int i = 0; i < size; i++)
        {
            cJSON* metric = cJSON_GetArrayItem(metrics, i);
            if (cJSON_IsString(metric))
            {
                printf("Metric: %s\n", metric->valuestring);
                configurations->metrics[i] = strdup(metric->valuestring);
            }
        }
    }

    cJSON_Delete(json);
    free(data);
}
/**
 * @brief updates the config.json file with the new metrics
 * and the new metrics count
 * 
 * @param filename 
 * @param new_metrics 
 * @param new_metrics_count 
 */
void update_config_json(const char* filename, char** new_metrics, int new_metrics_count)
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        perror("Unable to open config file");
        return;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* data = (char*)malloc(length + 1);
    fread(data, 1, length, file);
    fclose(file);
    data[length] = '\0';

    cJSON* json = cJSON_Parse(data);
    if (!json)
    {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        free(data);
        return;
    }

    cJSON* metrics = cJSON_GetObjectItem(json, "metrics");

    if (cJSON_IsArray(metrics))
    {
        cJSON_DeleteItemFromObject(json, "metrics");
        cJSON* new_metrics_array = cJSON_CreateArray();
        for (int i = 0; i < new_metrics_count; i++)
        {
            printf("New metric: %s\n", new_metrics[i]);
            cJSON_AddItemToArray(new_metrics_array, cJSON_CreateString(new_metrics[i]));
        }
        cJSON_AddItemToObject(json, "metrics", new_metrics_array);
    }

    char* new_data = cJSON_Print(json);
    file = fopen(filename, "w");
    if (!file)
    {
        perror("Unable to open config file");
        return;
    }
    fprintf(file, "%s", new_data);
    fclose(file);

    cJSON_Delete(json);
    free(data);
    free(new_data);
}