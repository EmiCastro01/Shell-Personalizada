#pragma once

#define HOST_NAME "survivor@refugio:"
#define TRUE 1
#define MAX_LINE_COMMAND 256
#define OCTAL_CLEAR "\033[H\033[J"

#define MONITOR_PATH "../monitor/metrics"

typedef struct {
  int sampling_interval;
  char **metrics;
} config_t;

/**
 * @brief 
 * 
 */
typedef enum
{
  MAIN_MODE,
  BACKGROUND_MODE,
} bg_mode_t;

/**
 * @brief 
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

typedef struct 
{
  run_mode_t mode;
  char *argument; 
} run_mode_struct_t;

/**
 * @brief 
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
cmd_t get_cmd(char **args);

/**
 * @brief Run the command
 * 
 * @param cmd 
 * @param args 
 */
void run_cmd(cmd_t cmd, char **args, config_t configurations);

/**
 * @brief Check if the arguments are not null
 * 
 */
run_mode_struct_t check_mode(int argc, char *argv[]);

/**
 * @brief 
 * 
 */
extern void config_signals_handlers(void);
/**
 * @brief 
 * 
 */
extern void sign_int_handler(int signal);

/**
 * @brief 
 * 
 */
extern void sign_quit_handler(int signal);

/**
 * @brief 
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
extern void execute_process(char **args, int input_fd, int output_fd, bg_mode_t bg_mode);

/**
 * @brief 
 * 
 */
extern void check_and_print_monitor_status(pid_t monitor_pid);

/**
 * @brief loads the configuration from the config.json file 
 * 
 */
void load_config_json(const char *filename, config_t *configurations);

extern int foreground_pid;

