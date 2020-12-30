set(MODULE_NAME util_network)

set(SRC
        tool.cpp
        buffer.cpp
        ssl_info.cpp
        tcp_pack.cpp
        session.cpp
        server.cpp
        )

add_library(${MODULE_NAME} STATIC ${SRC})

# third party 족 참조해야됨

