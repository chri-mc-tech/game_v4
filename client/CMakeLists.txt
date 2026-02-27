add_executable(client
        src/client_main.cpp
        ../shared/src/shared_utils.cpp
        ../shared/include/shared_utils.h
        include/client_global.h
        src/client_player.cpp
        include/client_player.h
        src/client_core.cpp
        include/client_core.h
        src/client_network.cpp
        include/client_network.h
        src/client_ui.cpp
        include/client_ui.h
)

target_include_directories(client PRIVATE include)
target_link_libraries(client PRIVATE shared)
