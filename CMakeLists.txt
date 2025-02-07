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
  GIT_TAG main # Important: Specify a version or a commit hash here.
)
FetchContent_MakeAvailable(ftxui)

# Compiler warnings and optimization flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -Wnull-dereference -Wold-style-cast -Wuseless-cast -Wdouble-promotion -Wzero-as-null-pointer-constant -Wstrict-null-sentinel -Wswitch-enum -Wswitch-default -Wunreachable-code -Wundef")

# Shared common library
file(GLOB_RECURSE COMMON_SRC_FILES "src/common/tetris.cpp" "src/common/*.hpp")
add_library(TetrisRoyale_Common ${COMMON_SRC_FILES})
target_include_directories(TetrisRoyale_Common PUBLIC "src/common")

# Terminal executable setup (using FTXUI)
# find_package(ftxui 5 COMPONENTS screen dom component REQUIRED)

file(GLOB_RECURSE TERMINAL_SRC_FILES "src/terminal/*.hpp" "src/terminal/main.cpp")
add_executable(TetrisRoyale_Terminal ${TERMINAL_SRC_FILES})
target_link_libraries(TetrisRoyale_Terminal PRIVATE TetrisRoyale_Common ftxui::screen ftxui::dom ftxui::component)
target_include_directories(TetrisRoyale_Terminal PRIVATE "src/terminal")

# Placeholder for additional executables

# Master Server executable
file(GLOB SERVER_SRC_FILES "src/server/*.cpp")
add_executable(TetrisRoyale_Server ${SERVER_SRC_FILES})
target_link_libraries(TetrisRoyale_Server PRIVATE TetrisRoyale_Common)

# DB Server executable
file(GLOB DB_SERVER_SRC_FILES "src/server/db-server/*.cpp" "src/server/db-server/*.hpp")
add_executable(TetrisRoyale_DB_Server ${DB_SERVER_SRC_FILES})
target_link_libraries(TetrisRoyale_DB_Server PRIVATE TetrisRoyale_Common crypto)

# Client-side executable
# file(GLOB CLIENT_SRC_FILES "src/client/*.cpp")
# add_executable(TetrisRoyale_Client ${CLIENT_SRC_FILES})
# target_link_libraries(TetrisRoyale_Client PRIVATE TetrisRoyale_Common)

# GUI-side executable
# file(GLOB GUI_SRC_FILES "src/gui/*.cpp")
# add_executable(TetrisRoyale_GUI ${GUI_SRC_FILES})
# target_link_libraries(TetrisRoyale_GUI PRIVATE TetrisRoyale_Common)