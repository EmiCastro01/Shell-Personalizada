#pragma once

#define HOST_NAME "survivor@refugio: "
#define TRUE 1
#define MAX_LINE_COMMAND 256

/**
 * @brief 
 * 
 */
char *pwd;
char *oldpwd;
/**
 * @brief enum to represent the commands
 * 
 */
typedef enum
{
  QUIT,
  CD,
  PWD,
  ECHO,
  CLR,
  NOP,
} cmd_t;
/**
 * @brief Executes the shell process
 * 
 */
void run_shell(void);

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
 * 
 */
void run_cmd(cmd_t cmd, char **args);