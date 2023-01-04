cmake_minimum_required(VERSION 3.16)

project(lab6 LANGUAGES CXX)

find_package(PkgConfig)
pkg_check_modules(PC_ZeroMQ QUIET zmq)
find_path(ZeroMQ_INCLUDE_DIR
        NAMES zmq.hpp
        PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
        )

find_library(ZeroMQ_LIBRARY
        NAMES zmq
        PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
        )

add_executable(main ./src/main.cpp)
add_executable(node ./src/node.cpp)

target_include_directories(main PUBLIC ${ZeroMQ_INCLUDE_DIR})
target_include_directories(node PUBLIC ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(main PUBLIC ${ZeroMQ_LIBRARY})
target_link_libraries(node PUBLIC ${ZeroMQ_LIBRARY})
