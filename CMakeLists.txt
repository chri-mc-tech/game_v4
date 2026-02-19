cmake_minimum_required(VERSION 3.30)
project(game_v4)

set(CMAKE_CXX_STANDARD 20)


add_subdirectory(shared)
add_subdirectory(client)
add_subdirectory(server)

find_package(unofficial-enet CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(cryptopp CONFIG REQUIRED)
find_package(sdl3 CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

target_link_libraries(server PRIVATE
        unofficial::enet::enet
        yaml-cpp::yaml-cpp
        cryptopp::cryptopp
)

target_link_libraries(client PRIVATE
        unofficial::enet::enet
        yaml-cpp::yaml-cpp
        cryptopp::cryptopp
        SDL3::SDL3
        SDL3_ttf::SDL3_ttf
)

target_link_libraries(shared PRIVATE
        unofficial::enet::enet
        yaml-cpp::yaml-cpp
        cryptopp::cryptopp
)

