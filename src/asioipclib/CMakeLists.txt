cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)
set(MODULE_NAME asio_ipc_lib)

set(SOURCES
        ConnectionManager.cpp
        SocketIpcServer.cpp
        SocketIpcClient.cpp
        SocketIpcCore.cpp
        MessageDefine.hpp
        ../include/common/StringTool.cpp
        ../include/common/FileDescriptorTool.cpp
        ../include/common/EpollWrapper.cpp
        ../include/common/SocketWrapper.cpp
        )
        #UnixDomainSocketFactoryBase.h)
include_directories(${ASIO_IPC} ${COMMON_PATH})

add_library(${MODULE_NAME} STATIC ${SOURCES})
find_package(Threads REQUIRED)

target_link_libraries(${MODULE_NAME}
        ${CMAKE_THREAD_LIBS_INIT})
