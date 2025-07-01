#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void recursive_search(const char* directory)
{
    DIR* dir = opendir(directory);
    if (!dir)
    {
        perror("opendir");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);

        struct stat statbuf;
        if (stat(path, &statbuf) == -1)
        {
            perror("stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode))
        {
            recursive_search(path); // Directorio: llamada recursiva
        }
        else if (S_ISREG(statbuf.st_mode))
        {
            // Archivo regular: verificar extensión
            size_t len = strlen(entry->d_name);
            if ((len > 7 && strcmp(entry->d_name + len - 7, ".config") == 0) ||
                (len > 5 && strcmp(entry->d_name + len - 5, ".json") == 0))
            {
                printf("Found config file: %s\n", path);
            }
        }
    }

    closedir(dir);
}
