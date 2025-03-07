# My classic / standard CMakeLists.txt template (Mihai)
# - Made in 2023

cmake_minimum_required(VERSION 3.22)

# Project name and standard
project(TetrisRoyale LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

# OpenSSL
find_package(OpenSSL QUIET)
if(NOT OpenSSL_FOUND)
    message(STATUS "OpenSSL not found locally. Please install OpenSSL development libraries.")
endif()

# FTXUI
include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG main
)
FetchContent_MakeAvailable(ftxui)

# Nlohmann JSON
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found locally, fetching using FetchContent...")

    include(FetchContent)
    FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.10.5
    )
    FetchContent_MakeAvailable(json)

endif()

# Boost
find_package(Boost QUIET REQUIRED COMPONENTS filesystem system)
if(NOT Boost_FOUND)
    message(STATUS "Boost not found locally, fetching using FetchContent...")

    include(FetchContent)
    FetchContent_Declare(
        boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG boost-1.87.0
    )
    FetchContent_MakeAvailable(boost)

    set(BOOST_INCLUDE_DIR ${boost_SOURCE_DIR})
else()
    message(STATUS "Found local Boost installation")
endif()

# Compiler warnings and optimization flags
add_compile_options(
        -pedantic -Wall -Wextra -O2 -Wformat=2 -Wfloat-equal -Wconversion
        -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align
        -Wnull-dereference -Wold-style-cast -Wuseless-cast -Wdouble-promotion
        -Wzero-as-null-pointer-constant -Wstrict-null-sentinel
        -Wswitch-default -Wunreachable-code -Wundef
        -Wno-null-dereference
)

# Shared common library
file(GLOB_RECURSE COMMON_SRC_FILES
        "src/common/*.cpp"
        "include/common/*.hpp"
)
add_library(TetrisRoyaleCommon STATIC ${COMMON_SRC_FILES})
target_include_directories(TetrisRoyaleCommon
        PUBLIC
        "include/common"
)

# Server game logic library
file(GLOB_RECURSE SERVER_LOGIC_SRC_FILES
        "src/server/server-game-logic/*.cpp"
        "include/server/server-game-logic/*.hpp"
)
add_library(TetrisRoyaleServerLogic STATIC ${SERVER_LOGIC_SRC_FILES})
target_include_directories(TetrisRoyaleServerLogic
        PUBLIC
        "include/server"
        "include/server/server-game-logic"
)
target_link_libraries(TetrisRoyaleServerLogic PRIVATE TetrisRoyaleCommon)

# Terminal executable setup
file(GLOB_RECURSE TERMINAL_SRC_FILES
        "src/client/terminal/view/*.hpp"
        "src/client/terminal/view/*.cpp"
        "src/client/terminal/main.cpp"
)
add_executable(TetrisRoyaleTerminal ${TERMINAL_SRC_FILES})
target_include_directories(TetrisRoyaleTerminal PRIVATE "src/client/terminal")
target_link_libraries(TetrisRoyaleTerminal
        PRIVATE
        TetrisRoyaleCommon
        TetrisRoyaleServerLogic
        ftxui::screen
        ftxui::dom
        ftxui::component
)

# TODO: Add
# Server common library
# file(GLOB SERVER_COMMON_SRC_FILES "src/common/server-common/*.cpp")
# add_library(TetrisRoyaleCommonServer ${SERVER_COMMON_SRC_FILES})
# target_include_directories(TetrisRoyaleCommonServer
#     PUBLIC
#         "include/common"
#         "include/common/server-common"
#     PRIVATE
#         ${boost_SOURCE_DIR}
# )
# target_compile_definitions(TetrisRoyaleCommonServer PRIVATE BOOST_UUID_HEADER_ONLY)
# target_link_libraries(TetrisRoyaleCommonServer
#     PRIVATE
#         nlohmann_json
# )

# HTTP Server library, used in the DB server, since no executable
# file(GLOB HTTP_SERVER_SRC_FILES "src/server/http-server/*.cpp")
# add_library(TetrisRoyaleHTTP STATIC ${HTTP_SERVER_SRC_FILES})
# target_include_directories(TetrisRoyaleHTTP PUBLIC "include/server/http-server")
#
# # DB Server library now, no more main()
# file(GLOB DB_SERVER_SRC_FILES "src/server/db-server/*.cpp")
# add_library(TetrisRoyaleDBServer STATIC ${DB_SERVER_SRC_FILES})
# target_include_directories(TetrisRoyaleDBServer
#     PUBLIC
#         "include/server/db-server"
#     PRIVATE
#         ${boost_SOURCE_DIR}
# )
# target_compile_definitions(TetrisRoyaleDBServer PRIVATE BOOST_UUID_HEADER_ONLY)
# target_link_libraries(TetrisRoyaleDBServer
#     PRIVATE
#         TetrisRoyaleCommonServer
#         TetrisRoyaleHTTP
#         sqlite3
#         crypto
# )

# Master Server executable
# file(GLOB TETRIS_MASTER_SERVER_SRC_FILES
#     "src/server/tetris-server/*.cpp"
#     "src/server/masterServer.cpp"
# )
# add_executable(TetrisRoyaleMasterServer ${TETRIS_MASTER_SERVER_SRC_FILES})
# target_include_directories(TetrisRoyaleMasterServer
#     PUBLIC
#         "include/server"
#         "include/server/db-server"
#         "include/server/http-server"
#         "include/server/tetris-server"
# )
# target_link_libraries(TetrisRoyaleMasterServer
#     PRIVATE
#         TetrisRoyaleCommonServer#
#         TetrisRoyaleHTTP
#         TetrisRoyaleDBServer
#         sqlite3
#         crypto
#         nlohmann_json
# )

# Client executable
# file(GLOB CLIENT_SRC_FILES "src/client/*.cpp")
# add_executable(TetrisRoyaleClient ${CLIENT_SRC_FILES})
# target_include_directories(TetrisRoyaleClient
#     PUBLIC
#         "include/client"
#         "include/common"
#         "include/common/server-common"
#         "include/server"
#     PRIVATE
#         ${boost_SOURCE_DIR}
# )
# target_compile_definitions(TetrisRoyaleClient PRIVATE
#     BOOST_ASIO_HEADER_ONLY
#     BOOST_BEAST_HEADER_ONLY
# )
# target_link_libraries(TetrisRoyaleClient
#     PRIVATE
#         TetrisRoyaleCommonServer
#         nlohmann_json
# )
