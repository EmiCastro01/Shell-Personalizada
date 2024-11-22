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
        configurations->metrics_count = size;
        for (int i = 0; i < size; i++) {
            cJSON *metric = cJSON_GetArrayItem(metrics, i);
            if (cJSON_IsString(metric)) {
                printf("Metric: %s\n", metric->valuestring);
                configurations->metrics[i] = strdup(metric->valuestring);
            }
        }
    }

    cJSON_Delete(json);
    free(data);
}

void update_config_json(const char *filename, char **new_metrics, int new_metrics_count)
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

    cJSON *metrics = cJSON_GetObjectItem(json, "metrics");

    if (cJSON_IsArray(metrics)) {
        cJSON_DeleteItemFromObject(json, "metrics");
        cJSON *new_metrics_array = cJSON_CreateArray();
        for (int i = 0; i < new_metrics_count; i++) {
            printf("New metric: %s\n", new_metrics[i]);
            cJSON_AddItemToArray(new_metrics_array, cJSON_CreateString(new_metrics[i]));
        }
        cJSON_AddItemToObject(json, "metrics", new_metrics_array);
    }

    char *new_data = cJSON_Print(json);
    file = fopen(filename, "w");
    if (!file) {
        perror("Unable to open config file");
        return;
    }
    fprintf(file, "%s", new_data);
    fclose(file);

    cJSON_Delete(json);
    free(data);
    free(new_data);
}
// Arreglar el tema del background mode. Funciona, pero no como piden
// Terminar el punto 6
