cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(
  test_shell
  VERSION 1.0.0
  DESCRIPTION "Test for shell project"
  LANGUAGES C
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage --coverage" )
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage" )

add_executable(test_shell
    test_shell.c
    ../src/shell.c
    ../src/util.c
)
target_link_libraries(test_shell PRIVATE cjson::cjson unity::unity)

add_test(NAME test_shell COMMAND test_shell)