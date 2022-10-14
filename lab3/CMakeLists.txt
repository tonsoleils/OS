cmake_minimum_required(VERSION 3.10)
project(lab3)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

find_library(PTHREAD_LIBRARY pthread)

add_executable(lab3
        src/main.cpp)

target_link_libraries(lab3 ${PTHREAD_LIBRARY})