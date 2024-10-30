#include <stdlib.h>
#include <stdio.h>
#include "shell.h"

int main(int argc, char *argv[])
{
    run_shell(check_args(argc, argv));
    return EXIT_SUCCESS;
}

run_mode_struct_t check_args(int argc, char *argv[])
{
    run_mode_struct_t run_mode;
    if (argc == 1)
    {
        run_mode.mode = INTERACTIVE_MODE;
        run_mode.argv = NULL;
        run_mode.argc = 0;
    }
    else
    {
        printf("Batch mode\n");
        run_mode.mode = BATCH_MODE;
        run_mode.argv = argv; // Almacenar el array de argumentos completo
        run_mode.argc = argc;
        printf("%s\n", argv[1]);
    }

    return run_mode;
}