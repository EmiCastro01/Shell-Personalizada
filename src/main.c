#include <stdlib.h>
#include <stdio.h>
#include "shell.h"
#include "util.c"

int main(int argc, char *argv[])
{
    run_shell(check_mode(argc, argv));
    return EXIT_SUCCESS;
}





