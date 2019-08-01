cmake_minimum_required(VERSION 2.4)
project(multic C)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -g -ggdb -lm")
set(CMAKE_C_STANDARD 99)
# for windows:
# for mac/linux:
set(LINK_DIR /usr/local/lib)
include_directories(${INC_DIR})
link_directories(${LINK_DIR})
set(LIBRARY_OUTPUT_PATH ./obj)
add_executable(multic ./src/main.c ./src/multic.c ./src/multic.h)

target_link_libraries(multic)
