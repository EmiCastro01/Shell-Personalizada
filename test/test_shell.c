#include "unity.h"
#include "../include/shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <cjson/cJSON.h>

void setUp(void) {
    // Configuración antes de cada prueba
}

void tearDown(void) {
    char *clean_metrics[] = {};
    update_config_json("../../config.json", clean_metrics, 0);    
}


void test_update_and_load_config_json(void) {
    char *new_metrics[] = {"cpu_usage", "IO_time"};
    update_config_json("../../config.json", new_metrics, 2);

    config_t configurations;
    load_config_json("../../config.json", &configurations);

    TEST_ASSERT_EQUAL_STRING("cpu_usage", configurations.metrics[0]);
    TEST_ASSERT_EQUAL_STRING("IO_time", configurations.metrics[1]);
}

void test_run_local_cmds(void) {
    tearDown();
    config_t configurations;
    load_config_json("../../config.json", &configurations);
    char *args[] = {"", "", NULL};
    args[0] = "echo";
    int result = run_cmd(ECHO, args, &configurations);
    TEST_ASSERT_EQUAL(0, result);
    args[0] = "clr";
    result = run_cmd(CLR, args, &configurations);
    TEST_ASSERT_EQUAL(0, result);
    

    tearDown();
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_update_and_load_config_json);
    RUN_TEST(test_run_local_cmds);

    tearDown();
    return UNITY_END();
}
