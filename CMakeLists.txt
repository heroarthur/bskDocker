cmake_minimum_required(VERSION 3.8)
project(untitled)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main_client.cpp client_utils/client_utils.h client_utils/client_utils.cpp client_utils/client_clock.cpp client_utils/client_clock.h)
add_executable(siktacka-client ${SOURCE_FILES})