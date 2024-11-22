#include <stdlib.h>
#include <stdio.h>
#include "shell.h"

int main(int argc, char *argv[])
{

 
    config_t configurations;
    load_config_json("../config.json", &configurations);
    run_shell(check_mode(argc, argv), configurations);
    return EXIT_SUCCESS;
}

