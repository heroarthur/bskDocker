cmake_minimum_required(VERSION 3.8)
project(untitled)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main_client.cpp client_utils/client/client_utils.h client_utils/client/client_utils.cpp client_utils/client/client_clock.cpp client_utils/client/client_clock.h client_utils/client/client.cpp client_utils/client/client.h client_utils/datagram/datagram.cpp client_utils/datagram/datagram.h client_utils/datagram/client_datagram.cpp client_utils/datagram/server_datagram.cpp main_server.cpp client_utils/server/server.cpp client_utils/server/server.h)
add_executable(siktacka-client ${SOURCE_FILES})

#add_executable(siktacka-client main_client.cpp)
#add_executable(serdver main_server.cpp)
