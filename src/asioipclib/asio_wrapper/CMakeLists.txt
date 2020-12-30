set(MODULE_NAME util_network)

set(SRC
        error_code.cpp
        tool.cpp
        buffer.cpp
        ssl_info.cpp
        tcp_pack.cpp
        session.cpp
        callback_timer.cpp
        server.cpp
        )

add_library(${MODULE_NAME} STATIC ${SRC})
target_link_libraries(${MODULE_NAME}
        LIB_PKG::boost
        LIB_PKG::openssl
        LIB_PKG::glog
)
#		CONAN_boost_system boost_chrono ssl crypto dl pthread)
