#include <stdlib.h>
#include <stdio.h>
#include "shell.h"
#include <cjson/cJSON.h>

int main(int argc, char *argv[])
{

 
    config_t configurations;
    load_config_json("../config.json", &configurations);
    run_shell(check_mode(argc, argv), configurations);
    return EXIT_SUCCESS;
}

void load_config_json(const char *filename, config_t *configurations)
{
     FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open config file");
        return;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = (char *)malloc(length + 1);
    fread(data, 1, length, file);
    fclose(file);
    data[length] = '\0';

    cJSON *json = cJSON_Parse(data);
    if (!json) {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        free(data);
        return;
    }

    cJSON *interval = cJSON_GetObjectItem(json, "sampling_interval");
    cJSON *metrics = cJSON_GetObjectItem(json, "metrics");

    if (cJSON_IsNumber(interval)) {
        printf("Sampling interval: %d\n", interval->valueint);
        configurations->sampling_interval = interval->valueint;
    }

    if (cJSON_IsArray(metrics)) {
        int size = cJSON_GetArraySize(metrics);
        for (int i = 0; i < size; i++) {
            cJSON *metric = cJSON_GetArrayItem(metrics, i);
            if (cJSON_IsString(metric)) {
                printf("Metric: %s\n", metric->valuestring);
                
            }
        }
    }

    cJSON_Delete(json);
    free(data);
}


// Arreglar el tema del background mode. Funciona, pero no como piden
// Terminar el punto 6
