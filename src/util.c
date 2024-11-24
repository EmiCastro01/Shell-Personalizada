#include "shell.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * False value
 */
#define FALSE 0
/**
 * True value
 */
#define TRUE 1
/**
 * foreground PID
 */
int foreground_pid = -1; // global variable to save the foreground process PID

/**
 * @brief returns the mode of the shell based on the arguments
 *
 * @param argc
 * @param argv
 * @return run_mode_struct_t
 */
run_mode_struct_t check_mode(int argc, char* argv[])
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

/**
 * @brief Check if process needs to run in background
 *
 */
bg_mode_t check_bg(char** args)
{
    bg_mode_t background = MAIN_MODE;
    int arg_index = 0;
    while (args[arg_index] != NULL)
    {
        size_t len = strlen(args[arg_index]);
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

/**
 * @brief Signal handler for SIGINT
 *
 * @param signal
 */
void sign_int_handler(int signal)
{
    if (foreground_pid > 0)
    {
        kill(foreground_pid, SIGINT);
    }
}
/**
 * @brief Signal handler for SIGQUIT
 *
 * @param signal
 */
void sign_quit_handler(int signal)
{
    if (foreground_pid > 0)
    {
        kill(foreground_pid, SIGQUIT);
    }
}
/**
 * @brief Signal handler for SIGTSTP
 *
 * @param signal
 */

void sign_stop_handler(int signal)
{
    if (foreground_pid > 0)
    {
        kill(foreground_pid, SIGTSTP);
    }
}
/**
 * @brief Signal handlers configuration
 *
 */
void config_signals_handlers()
{
    signal(SIGINT, sign_int_handler);
    signal(SIGQUIT, sign_quit_handler);
    signal(SIGTSTP, sign_stop_handler);
}
/**
 * @brief Check and print the status of the monitor
 *
 * @param monitor_pid
 */
void check_and_print_monitor_status(pid_t monitor_pid)
{
    if (monitor_pid == -1)
    {
        printf("Monitor is not running\n");
    }
    else
    {
        printf("Monitor is running with PID %d\n", monitor_pid);
    }
}