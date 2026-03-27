cmake_minimum_required(VERSION 3.30)
project(game_v4)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_CLIENT "Build client executable" ON)

find_package(unofficial-enet CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(cryptopp CONFIG REQUIRED)

if(BUILD_CLIENT)
    find_package(sdl3 CONFIG REQUIRED)
    find_package(SDL3_ttf CONFIG REQUIRED)
    find_package(raylib CONFIG REQUIRED)
    find_package(glfw3 CONFIG REQUIRED)
endif()

add_subdirectory(shared)

if(BUILD_CLIENT)
    add_subdirectory(client)
endif()

add_subdirectory(server)

target_link_libraries(server PRIVATE
        unofficial::enet::enet
        yaml-cpp::yaml-cpp
        cryptopp::cryptopp
)

if(BUILD_CLIENT)
    target_link_libraries(client PRIVATE
            unofficial::enet::enet
            yaml-cpp::yaml-cpp
            cryptopp::cryptopp
            SDL3::SDL3
            SDL3_ttf::SDL3_ttf
            raylib
            glfw
    )
endif()

target_link_libraries(shared PRIVATE
        unofficial::enet::enet
        yaml-cpp::yaml-cpp
        cryptopp::cryptopp
)