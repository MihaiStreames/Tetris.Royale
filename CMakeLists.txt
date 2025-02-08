# My classic / standard CMakeLists.txt template (Mihai)
# - Made in 2023

# Don't forget to increment this as new CMake versions come out :))
cmake_minimum_required(VERSION 3.30)

# Project name and standard
project(TetrisRoyale LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG main
)
FetchContent_MakeAvailable(ftxui)

# Compiler warnings and optimization flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -Wnull-dereference -Wold-style-cast -Wuseless-cast -Wdouble-promotion -Wzero-as-null-pointer-constant -Wstrict-null-sentinel -Wswitch-enum -Wswitch-default -Wunreachable-code -Wundef")

# Shared common library
file(GLOB_RECURSE COMMON_SRC_FILES
    "src/common/tetris.cpp"
    "src/common/*.hpp"
)
add_library(TetrisRoyaleCommon ${COMMON_SRC_FILES})
target_include_directories(TetrisRoyaleCommon PUBLIC "src/common")

# HTTP Server library
file(GLOB HTTP_SERVER_SRC_FILES
    "src/server/http-server/*.cpp"
    "src/server/http-server/*.hpp"
)
add_library(TetrisRoyaleHTTP STATIC ${HTTP_SERVER_SRC_FILES})
target_include_directories(TetrisRoyaleHTTP PUBLIC "src/server/http-server")
target_link_libraries(TetrisRoyaleHTTP
    PRIVATE
        TetrisRoyaleCommon
)

# UDP Server library
file(GLOB UDP_SERVER_SRC_FILES
    "src/server/udp-server/*.cpp"
    "src/server/udp-server/*.hpp"
)
add_library(TetrisRoyaleUDP STATIC ${UDP_SERVER_SRC_FILES})
target_include_directories(TetrisRoyaleUDP PUBLIC "src/server/udp-server")
target_link_libraries(TetrisRoyaleUDP
    PRIVATE
        TetrisRoyaleCommon
)

# DB Server executable
file(GLOB DB_SERVER_SRC_FILES
    "src/server/db-server/*.cpp"
    "src/server/db-server/*.hpp"
)
add_executable(TetrisRoyaleDBServer ${DB_SERVER_SRC_FILES})
target_link_libraries(TetrisRoyaleDBServer
    PRIVATE
        TetrisRoyaleCommon
        TetrisRoyaleHTTP
        crypto
)

# Master Server executable
file(GLOB MASTER_SERVER_SRC_FILES
    "src/server/master-server/*.cpp"
    "src/server/master-server/*.hpp"
    "src/server/server.cpp"
)
add_executable(TetrisRoyaleMasterServer ${MASTER_SERVER_SRC_FILES})
target_link_libraries(TetrisRoyaleMasterServer
    PRIVATE
        TetrisRoyaleCommon
        TetrisRoyaleHTTP
        TetrisRoyaleUDP
)

# Terminal executable setup
file(GLOB_RECURSE TERMINAL_SRC_FILES
        "src/terminal/*.hpp"
        "src/terminal/main.cpp"
)
add_executable(TetrisRoyaleTerminal ${TERMINAL_SRC_FILES})
target_link_libraries(TetrisRoyaleTerminal
        PRIVATE
        TetrisRoyaleCommon
        ftxui::screen
        ftxui::dom
        ftxui::component
)
target_include_directories(TetrisRoyaleTerminal PRIVATE "src/terminal")

# Placeholder for additional executables
# Client-side executable
# file(GLOB CLIENT_SRC_FILES "src/client/*.cpp")
# add_executable(TetrisRoyale_Client ${CLIENT_SRC_FILES})
# target_link_libraries(TetrisRoyale_Client PRIVATE TetrisRoyale_Common)

# GUI-side executable
# file(GLOB GUI_SRC_FILES "src/gui/*.cpp")
# add_executable(TetrisRoyale_GUI ${GUI_SRC_FILES})
# target_link_libraries(TetrisRoyale_GUI PRIVATE TetrisRoyale_Common)