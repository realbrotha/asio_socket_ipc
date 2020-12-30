project(TestEpollPipe)
cmake_minimum_required(VERSION 3.5)

set(BIN_PATH ${CMAKE_CURRENT_BINARY_DIR}/output_bin)
set(COMMON_PATH src/include/common)
set(ASIO_IPC ${CMAKE_CURRENT_SOURCE_DIR}/src/asioipclib )
set(BOOST_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/src/include)

include_directories(${BOOST_INCLUDE})

add_subdirectory(src/asioipclib)
add_subdirectory(src/client)
add_subdirectory(src/server)
