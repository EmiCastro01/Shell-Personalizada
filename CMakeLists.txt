# shell-personalizada/CMakeLists.txt

cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(
  "ShellProject"
  VERSION 1.0.0
  DESCRIPTION "A nice shell project!"
  LANGUAGES C)

# enable testing
include(CTest)

# flags for compiler
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wconversion")

# include headers
include_directories(include)

# dependencies
find_package(cJSON REQUIRED)

add_subdirectory(monitor)

# 🔧 Crear la LIBRERÍA REUTILIZABLE llamada shell_lib
add_library(shell_lib STATIC src/shell.c src/util.c src/config_search.c)

target_include_directories(shell_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(shell_lib PRIVATE cjson::cjson)

# 🔧 El ejecutable de shell (si querés seguir teniéndolo como TP2)
add_executable(ShellProject src/main.c)
target_link_libraries(ShellProject PRIVATE shell_lib)

if(RUN_COVERAGE EQUAL 1)
  message("run coverage")
  add_subdirectory(test)
endif()
