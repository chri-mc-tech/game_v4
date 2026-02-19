add_library(shared STATIC
        src/shared_utils.cpp
        src/shared_crypto.cpp
        include/shared_crypto.h
        src/shared_network.cpp
        include/shared_network.h
        src/shared_global.cpp
        include/shared_global.h
)

target_include_directories(shared PUBLIC include)

