add_executable(server
        src/server_main.cpp
        src/server_logger.cpp
        src/server_core.cpp
        src/server_console.cpp
        include/server_console.h
        include/server_global.h
        src/server_network.cpp
        include/server_network.h
        include/server_player.h
)

target_include_directories(server PRIVATE include)
target_link_libraries(server PRIVATE shared)
