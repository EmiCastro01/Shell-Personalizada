#pragma once

#define HOST_NAME "survivor@refugio:"
#define TRUE 1
#define MAX_LINE_COMMAND 256
#define OCTAL_CLEAR "\033[H\033[J"

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

typedef enum
{
  QUIT,
  CD,
  PWD,
  ECHO,
  CLR,
  EXTERNAL,
} cmd_t;

/**
 * @brief Executes the shell process
 * 
 */
void run_shell(run_mode_struct_t mode);

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
void run_cmd(cmd_t cmd, char **args);

/**
 * @brief Check if the arguments are not null
 * 
 */
run_mode_struct_t check_mode(int argc, char *argv[]);