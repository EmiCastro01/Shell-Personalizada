# cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(monitor_only LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wconversion")

# Incluir subdirectorio monitor
add_subdirectory(monitor)

# No ejecutables extras ni librerías que no uses
