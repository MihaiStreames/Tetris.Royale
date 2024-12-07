# My classic / standard CMakeLists.txt template (Mihai)
# - Made in 2023

# Don't forget to increment this as new CMake versions come out :))
cmake_minimum_required(VERSION 3.30)

# Project name and standard
project(TetrisRoyale)
set(CMAKE_CXX_STANDARD 23)

# Compiler warnings and optimization flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -Wnull-dereference -Wold-style-cast -Wuseless-cast -Wdouble-promotion -Wzero-as-null-pointer-constant -Wstrict-null-sentinel -Wswitch-enum -Wswitch-default -Wunreachable-code -Wundef")

# Sanitizers (optional, uncomment if needed)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined")

# Include directories
include_directories(include)

# TODO: Add FTXUI libs

# Find Allegro libraries (not needed for now - will probably use Qt)
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(ALLEGRO REQUIRED allegro-5 allegro_primitives-5 allegro_font-5 allegro_ttf-5)
# include_directories(${ALLEGRO_INCLUDE_DIRS})
# link_directories(${ALLEGRO_LIBRARY_DIRS})

# Source files
file(GLOB SRC_FILES "src/*.cpp")

# Executable target
add_executable(${PROJECT_NAME} ${SRC_FILES})
# target_link_libraries(${PROJECT_NAME} ${ALLEGRO_LIBRARIES} allegro_main)
# target_compile_options(${PROJECT_NAME} PRIVATE ${ALLEGRO_CFLAGS})