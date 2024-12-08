# My classic / standard CMakeLists.txt template (Mihai)
# - Made in 2023

cmake_minimum_required(VERSION 3.30)

# Project name and standard
project(TetrisRoyale)
set(CMAKE_CXX_STANDARD 23)

# Compiler warnings and optimization flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -Wnull-dereference -Wold-style-cast -Wuseless-cast -Wdouble-promotion -Wzero-as-null-pointer-constant -Wstrict-null-sentinel -Wswitch-enum -Wswitch-default -Wunreachable-code -Wundef")

# Include directories
include_directories(include)

# Shared common library
file(GLOB COMMON_SRC_FILES "src/common/*.cpp")
add_library(TetrisRoyale_Common ${COMMON_SRC_FILES})

# Terminal executable setup (using FTXUI)
find_package(ftxui REQUIRED)

file(GLOB TERMINAL_SRC_FILES "src/terminal/*.cpp")
add_executable(TetrisRoyale_Terminal ${TERMINAL_SRC_FILES})
target_link_libraries(TetrisRoyale_Terminal PRIVATE TetrisRoyale_Common ftxui::screen ftxui::dom ftxui::component)

# Placeholder for additional executables

# Server-side executable
# file(GLOB SERVER_SRC_FILES "src/server/*.cpp")
# add_executable(TetrisRoyale_Server ${SERVER_SRC_FILES})
# target_link_libraries(TetrisRoyale_Server PRIVATE TetrisRoyale_Common)

# Client-side executable
# file(GLOB CLIENT_SRC_FILES "src/client/*.cpp")
# add_executable(TetrisRoyale_Client ${CLIENT_SRC_FILES})
# target_link_libraries(TetrisRoyale_Client PRIVATE TetrisRoyale_Common)

# GUI-side executable
# file(GLOB GUI_SRC_FILES "src/gui/*.cpp")
# add_executable(TetrisRoyale_GUI ${GUI_SRC_FILES})
# target_link_libraries(TetrisRoyale_GUI PRIVATE TetrisRoyale_Common)