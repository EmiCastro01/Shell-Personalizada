#include "shell.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Check the mode of the shell
 * Main function
 * @param argc 
 * @param argv 
 */
int main(int argc, char* argv[])
{

    config_t configurations;
    load_config_json("../config.json", &configurations);
    run_shell(check_mode(argc, argv), configurations);
    return EXIT_SUCCESS;
}
