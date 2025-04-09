# This is a heavily revised version of mihai's CMake template,
# rewritten by antoine to be more modular and easier to maintain <3

# Project name and standard
cmake_minimum_required(VERSION 3.22...4.0)
project(TetrisRoyale VERSION 1.0.0 LANGUAGES CXX)

# Set project standard and options
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# options
option(TETRIS_USE_SANITIZERS "Enable sanitizers in debug mode" OFF)


# Append the environment variable to CMAKE_PREFIX_PATH

if(DEFINED ENV{CMAKE_PREFIX_PATH})
    list(APPEND CMAKE_PREFIX_PATH $ENV{CMAKE_PREFIX_PATH})
endif()

# ============================================
# Find Dependencies with System or Local Path
# ============================================

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# Function to find a package compatible with cmake, fallback to local if not found
macro(find_lib_or_exit lib_name env_var)
    
    # Check if the environment variable is defined
    if(DEFINED ENV{${env_var}})
        message(STATUS "Using locally installed ${lib_name} from $ENV{${env_var}}")
        set(${lib_file_var} $ENV{${env_var}} CACHE PATH "Path to ${lib_name}")
    endif()

    # then we use the standard find_package method
    find_package(${lib_name} REQUIRED)

    if(NOT ${lib_name}_FOUND)
        message(FATAL_ERROR "${lib_name} not found, and no valid custom path provided. Please install it or set ${env_var} correctly.")
    endif()

    message(STATUS "${lib_name} found!")

endmacro()


# Find dependencies
find_lib_or_exit("Boost" "BOOST_ROOT")
find_lib_or_exit("OpenSSL" "OPENSSL_ROOT")
find_lib_or_exit("SQLite3" "SQLITE_ROOT")
find_lib_or_exit("GTest" "GTEST_ROOT")


# TODO : Setup ftxui outside of the cmake (maybe? im super lazy)

# Setup JSON
function(setup_json)
    if (DEFINED ENV{NLOHMANN_JSON_ROOT} AND EXISTS "$ENV{NLOHMANN_JSON_ROOT}/include/nlohmann/json.hpp")
        message(STATUS "Using locally installed nlohmann_json from $ENV{NLOHMANN_JSON_ROOT}")
        include_directories("$ENV{NLOHMANN_JSON_ROOT}/include")
        add_library(nlohmann_json::nlohmann_json INTERFACE IMPORTED)
        set_target_properties(nlohmann_json::nlohmann_json PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "$ENV{NLOHMANN_JSON_ROOT}/include")
    else ()
        # Search for nlohmann_json in the system
        find_package(nlohmann_json REQUIRED)
        if (NOT nlohmann_json_FOUND)
            message(FATAL_ERROR "nlohmann_json not found, and no valid custom path provided. Please install it or set NLOHMANN_JSON_ROOT correctly.")
        endif()
    endif()

    message(STATUS "nlohmann_json found!")
    
endfunction()

# Setup FTXUI
function(setup_ftxui)
    # Try to find a pre-installed FTXUI
    # Note: FTXUI doesn't provide standard CMake config files,
    # so we'll need to fetch it if custom paths aren't provided
    if (DEFINED ENV{FTXUI_ROOT} AND EXISTS "$ENV{FTXUI_ROOT}/include/ftxui")
        message(STATUS "Using locally installed FTXUI from $ENV{FTXUI_ROOT}")
        # Create imported targets for FTXUI
        add_library(ftxui-screen STATIC IMPORTED)
        add_library(ftxui-dom STATIC IMPORTED)
        add_library(ftxui-component STATIC IMPORTED)

        set_target_properties(ftxui-screen PROPERTIES
                IMPORTED_LOCATION "$ENV{FTXUI_ROOT}/lib/libftxui-screen.a"
                INTERFACE_INCLUDE_DIRECTORIES "$ENV{FTXUI_ROOT}/include")

        set_target_properties(ftxui-dom PROPERTIES
                IMPORTED_LOCATION "$ENV{FTXUI_ROOT}/lib/libftxui-dom.a"
                INTERFACE_INCLUDE_DIRECTORIES "$ENV{FTXUI_ROOT}/include")

        set_target_properties(ftxui-component PROPERTIES
                IMPORTED_LOCATION "$ENV{FTXUI_ROOT}/lib/libftxui-component.a"
                INTERFACE_INCLUDE_DIRECTORIES "$ENV{FTXUI_ROOT}/include")

        # Create aliased targets
        add_library(ftxui::screen ALIAS ftxui-screen)
        add_library(ftxui::dom ALIAS ftxui-dom)
        add_library(ftxui::component ALIAS ftxui-component)

        # Add include directory globally
        include_directories("$ENV{FTXUI_ROOT}/include")
    else ()
        message(STATUS "FTXUI not found locally, fetching...")
        FetchContent_Declare(ftxui
                GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
                GIT_TAG v5.0.0
                GIT_SHALLOW ON
        )
        FetchContent_MakeAvailable(ftxui)
    endif ()
endfunction()

setup_json()
setup_ftxui()



# Build components
# ----------------------------------------------------------

# Compiler flags function
function(set_target_compile_options target)
    target_compile_options(${target} PRIVATE
            -pedantic -Wall -Wextra -Wformat=2 -Wfloat-equal -Wconversion
            -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align
            -Wnull-dereference -Wold-style-cast -Wuseless-cast -Wdouble-promotion
            -Wzero-as-null-pointer-constant -Wstrict-null-sentinel
            -Wswitch-default -Wunreachable-code -Wundef
            -Wno-null-dereference
    )

    # Configure build-type specific options
    target_compile_options(${target} PRIVATE
            $<$<CONFIG:Debug>:-O0 -g3 -ggdb>
            $<$<CONFIG:Release>:-O3 -march=native -DNDEBUG>
    )

    # Add sanitizers in debug mode if requested
    if (TETRIS_USE_SANITIZERS AND CMAKE_BUILD_TYPE MATCHES Debug)
        target_compile_options(${target} PRIVATE
                -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE
                -fsanitize=address,undefined)
    endif ()
endfunction()

# Compiler Libraries function
function(add_tetris_library name type sources includes)
    set(options "")
    set(oneValueArgs "")
    set(multiValueArgs libs)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_library(${name} ${type} ${sources})
    target_include_directories(${name} PUBLIC ${includes})

    if (ARG_libs)
        target_link_libraries(${name} PUBLIC ${ARG_libs})
    endif ()

    set_target_compile_options(${name})
endfunction()

# Compiler Executables function
function(add_tetris_executable name sources includes)
    set(options "")
    set(oneValueArgs "")
    set(multiValueArgs libs)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_executable(${name} ${sources})
    target_include_directories(${name} PUBLIC ${includes})

    if (ARG_libs)
        target_link_libraries(${name} PUBLIC ${ARG_libs})
    endif ()

    set_target_compile_options(${name})
endfunction()

# Include & Source directories
set(TETRIS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(TETRIS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Files
file(GLOB COMMON_SRC_FILES "${TETRIS_SRC_DIR}/common/*.cpp")
file(GLOB SERVER_COMMON_SRC_FILES "${TETRIS_SRC_DIR}/common/server-common/*.cpp")
file(GLOB SERVER_LOGIC_SRC_FILES "${TETRIS_SRC_DIR}/server/server-game-logic/*.cpp")
file(GLOB HTTP_SERVER_SRC_FILES "${TETRIS_SRC_DIR}/server/http-server/*.cpp")
file(GLOB DB_SERVER_SRC_FILES "${TETRIS_SRC_DIR}/server/db-server/*.cpp")
file(GLOB TETRIS_SERVER_SRC_FILES "${TETRIS_SRC_DIR}/server/tetris-server/*.cpp")
file(GLOB CLIENT_CONNECTIVITY_SRC_FILES "${TETRIS_SRC_DIR}/client/connectivity/*.cpp")
file(GLOB CLIENT_TUI_SRC_FILES "${TETRIS_SRC_DIR}/client/tui/*.cpp" "${TETRIS_SRC_DIR}/client/tui/menus/*.cpp")

# Libraries
add_tetris_library(TetrisRoyaleCommon "" "${COMMON_SRC_FILES}" "${TETRIS_INCLUDE_DIR}/common")
add_tetris_library(TetrisRoyaleCommonServer "" "${SERVER_COMMON_SRC_FILES}" "${TETRIS_INCLUDE_DIR}/common/server-common" libs TetrisRoyaleCommon Boost::boost OpenSSL::Crypto nlohmann_json::nlohmann_json)
add_tetris_library(TetrisRoyaleGameLogic STATIC "${SERVER_LOGIC_SRC_FILES}" "${TETRIS_INCLUDE_DIR}/server/server-game-logic" libs TetrisRoyaleCommon)
add_tetris_library(TetrisRoyaleHTTPServer STATIC "${HTTP_SERVER_SRC_FILES}" "${TETRIS_INCLUDE_DIR}/server/http-server" libs Boost::boost)
add_tetris_library(TetrisRoyaleDBServer STATIC "${DB_SERVER_SRC_FILES}" "${TETRIS_INCLUDE_DIR}/server/db-server" libs TetrisRoyaleHTTPServer TetrisRoyaleCommonServer TetrisRoyaleCommon Boost::boost SQLite::SQLite3 OpenSSL::Crypto)
add_tetris_library(TetrisRoyaleTetrisServer "" "${TETRIS_SERVER_SRC_FILES}" "${TETRIS_INCLUDE_DIR}/server/tetris-server" libs TetrisRoyaleGameLogic TetrisRoyaleCommonServer TetrisRoyaleCommon)
add_tetris_library(TetrisRoyaleClientConnectivity "" "${CLIENT_CONNECTIVITY_SRC_FILES}" "${TETRIS_INCLUDE_DIR}/client/connectivity" libs nlohmann_json::nlohmann_json TetrisRoyaleCommonServer TetrisRoyaleCommon Boost::boost)
add_tetris_library(TetrisRoyaleClientTUI "" "${CLIENT_TUI_SRC_FILES}" "${TETRIS_INCLUDE_DIR}/client/tui;${TETRIS_INCLUDE_DIR}/client/tui/menus" libs TetrisRoyaleClientConnectivity ftxui::screen ftxui::dom ftxui::component)

# Executables
add_tetris_executable(TetrisRoyaleMasterServer "${TETRIS_SRC_DIR}/server/MasterServer.cpp" "${TETRIS_INCLUDE_DIR}/server" libs TetrisRoyaleTetrisServer TetrisRoyaleDBServer TetrisRoyaleCommon)
add_tetris_executable(TetrisRoyaleClient "${TETRIS_SRC_DIR}/client/Client.cpp" "${TETRIS_INCLUDE_DIR}/client" libs TetrisRoyaleClientTUI TetrisRoyaleClientConnectivity)


# ==================================================== #
#                      Testing                         #
# ==================================================== #
function(add_tetris_test TEST_FILE)
  get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
  add_executable(${TEST_NAME} ${TEST_FILE})
  target_link_libraries(${TEST_NAME} TetrisRoyaleCommon TetrisRoyaleGameLogic GTest::GTest GTest::Main)
  add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

# Create a test executable for each test file in /tests directory
file(GLOB TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")
foreach(TEST_FILE ${TEST_FILES})
  add_tetris_test(${TEST_FILE})
endforeach()

enable_testing()  # Enable testing in CMake


# Print configuration summary
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "Testing enabled: ${TETRIS_ENABLE_TESTS}")
message(STATUS "Sanitizers enabled: ${TETRIS_USE_SANITIZERS}")

