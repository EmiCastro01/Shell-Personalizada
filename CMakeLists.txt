cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(
    "ShellProject"
    VERSION 1.0.0
    DESCRIPTION "A nice shell project!"
    LANGUAGES C
)

#enable testing
include(CTest)

#flags for compiler
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS_DEBUG "-g -00 -Wall -Wconversion")


#include headers
include_directories(include)

#find dependencies
find_package(cJSON REQUIRED)
find_package(Unity REQUIRED)

add_subdirectory(monitor)

# Ensure the main target depends on the monitor build
# add the executable  -> .exe or binary
add_executable(${PROJECT_NAME}
    src/main.c
    src/shell.c
    src/util.c
)


target_link_libraries(${PROJECT_NAME} PRIVATE cjson::cjson)

if(RUN_COVERAGE EQUAL 1)
  message("run coverage")
  add_subdirectory(test)
endif()






