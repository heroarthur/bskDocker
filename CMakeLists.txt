cmake_minimum_required(VERSION 3.8)
project(untitled)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main_client.cpp client_utils.h client_utils.cpp)
add_executable(siktacka-client ${SOURCE_FILES})