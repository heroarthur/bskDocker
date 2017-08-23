cmake_minimum_required(VERSION 3.8)
project(untitled)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main_client.cpp client_utils/client_utils.h client_utils/client_utils.cpp client_utils/client_clock.cpp client_utils/client_clock.h client_utils/client.cpp client_utils/client.h client_utils/message_for_gui.cpp client_utils/datagram.cpp client_utils/datagram.h client_utils/client_datagram.cpp)
add_executable(siktacka-client ${SOURCE_FILES})