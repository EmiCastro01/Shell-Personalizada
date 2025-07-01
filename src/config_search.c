#include "cJSON.h"
#include <cinttypes>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void print_json(const char* filepath);
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
                print_json(path);
            }
        }

        closedir(dir);
    }
}

void print_json(const char* filepath)
{
    FILE* file = fopen(filepath, "r");
    if (!file)
    {
        perror("fopen");
        return;
    }

    // Obtener tamaño del archivo
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    rewind(file);

    // Leer todo el contenido
    char* content = malloc(filesize + 1);
    if (!content)
    {
        fclose(file);
        perror("malloc");
        return;
    }

    fread(content, 1, filesize, file);
    content[filesize] = '\0';
    fclose(file);

    // Parsear con cJSON
    cJSON* root = cJSON_Parse(content);
    if (!root)
    {
        fprintf(stderr, "Error al parsear JSON: %s\n", cJSON_GetErrorPtr());
        free(content);
        return;
    }

    // Recorremos el objeto JSON (nivel raíz)
    cJSON* element = NULL;
    cJSON_ArrayForEach(element, root)
    {
        if (cJSON_IsString(element))
        {
            printf("clave: \"%s\"  valor: \"%s\"\n", element->string, element->valuestring);
        }
        else if (cJSON_IsNumber(element))
        {
            printf("clave: \"%s\"  valor: %f\n", element->string, element->valuedouble);
        }
        else if (cJSON_IsBool(element))
        {
            printf("clave: \"%s\"  valor: %s\n", element->string, cJSON_IsTrue(element) ? "true" : "false");
        }
        else
        {
            printf("clave: \"%s\"  valor: (tipo no manejado)\n", element->string);
        }
    }

    cJSON_Delete(root);
    free(content);
}
