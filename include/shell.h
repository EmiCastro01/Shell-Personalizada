#pragma once

#include <sys/types.h>
/**
 * host name
 */
#define HOST_NAME "survivor@refugio:"
/**
 * true value
 */
#define TRUE 1
/**
 * max line command
 */
#define MAX_LINE_COMMAND 256
/**
 * octal code for clearing screen and moving cursor to home
 */
#define OCTAL_CLEAR "\033[H\033[J"
/**
 * monitor subdirectory path
 */
#define MONITOR_PATH "./monitor/monitoring_project"
/**
 * max metrics
 */
#define MAX_METRICS 10

/**
 * @brief struct to store configurations
 * passed by config.json
 * or set by the user
 */
typedef struct
{
    /**
     * @brief variable to store the sampling interval
     */
    int sampling_interval;
    /**
     * @brief array of metrics
     */
    char* metrics[MAX_METRICS];
    /**
     * @brief number of metrics
     */
    int metrics_count;
} config_t;

/**
 * @brief Represent the mode of the process
 *
 */
typedef enum
{
    MAIN_MODE,
    BACKGROUND_MODE,
} bg_mode_t;

/**
 * @brief Check if the process is in background
 *
 */
bg_mode_t check_bg(char** args);

/**
 * @brief Represent the mode of shell
 *
 */
typedef enum
{
    INTERACTIVE_MODE,
    BATCH_MODE,
} run_mode_t;
/**
 * @brief struct to store the mode and the argument
 *
 */
typedef struct
{
    /**
     * @brief mode of the process
     */
    run_mode_t mode;
    /**
     * @brief argument of the process
     */
    char* argument;
} run_mode_struct_t;

/**
 * @brief Represent the command to be executed
 *
 */
typedef enum
{
    QUIT,
    CD,
    PWD,
    ECHO,
    CLR,
    START_MONITOR,
    STOP_MONITOR,
    STATUS_MONITOR,
    SHOW_CPU_USAGE,
    SHOW_MEM_USAGE,
    SHOW_DISK_USAGE,
    SHOW_PROC_NO,
    SHOW_NET_USAGE,
    EXTERNAL,
} cmd_t;

/**
 * @brief Executes the shell process
 *
 */
void run_shell(run_mode_struct_t mode, config_t configurations);

/**
 * @brief Get the program to be executed
 *
 * @param args
 */
cmd_t get_cmd(char** args);

/**
 * @brief Run the command
 *
 * @param cmd
 * @param args
 */
int run_cmd(cmd_t cmd, char** args, config_t* configurations);

/**
 * @brief Check if the arguments are not null
 *
 */
run_mode_struct_t check_mode(int argc, char* argv[]);

/**
 * @brief signal handlers configuration
 *
 */
extern void config_signals_handlers(void);
/**
 * @brief signal handler for SIGINT
 *
 */
extern void sign_int_handler(int signal);

/**
 * @brief signal handler for SIGQUIT
 *
 */
extern void sign_quit_handler(int signal);

/**
 * @brief signal handler for SIGTSTP
 *
 */
extern void sign_stop_handler(int signal);

/**
 * @brief takes info from the args and executes the process in background or main mode
 * @param args
 * @param input_fd
 * @param output_fd
 * @param is_background represents if the process is in background
 */
extern void execute_process(char** args, int input_fd, int output_fd, bg_mode_t bg_mode);

/**
 * @brief check and print the status of the monitor
 *
 */
extern void check_and_print_monitor_status(pid_t monitor_pid);

/**
 * @brief loads the configuration from the config.json file
 *
 */
void load_config_json(const char* filename, config_t* configurations);

/**
 * @brief updates the configuration in the config.json file
 *
 */
void update_config_json(const char* filename, char** new_metrics, int new_metrics_count);

/**
 * foreground process id
 */
extern int foreground_pid;
