# Project name and standard
cmake_minimum_required(VERSION 3.22...4.0)
project(TetrisRoyale VERSION 1.0.0 LANGUAGES CXX)

# Set project standard and options
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Packages
find_package(Qt5Widgets REQUIRED)

# Executables needed
add_executable(TetrisGame
    main.cpp
    LoginScreen.cpp
    RegisterScreen.cpp
    LeaderScreen.cpp
    resources/qrc_resources.cpp
)

# Libraries
target_link_libraries(TetrisGame Qt5::Widgets)