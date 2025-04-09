# This is a revised version of mihai's CMake template,
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

# Function to find a package, fallback to local if not found
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


# TODO : Setup this outside of the cmake

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

# Setup nlohmann_json
function(setup_json)
    find_package(nlohmann_json 3.10.5 QUIET)
    if (nlohmann_json_FOUND)
        message(STATUS "Found nlohmann_json: ${nlohmann_json_VERSION}")
    else ()
        # Check for environment variable pointing to local installation
        if (DEFINED ENV{NLOHMANN_JSON_ROOT} AND EXISTS "$ENV{NLOHMANN_JSON_ROOT}/include/nlohmann/json.hpp")
            message(STATUS "Using locally installed nlohmann_json from $ENV{NLOHMANN_JSON_ROOT}")
            include_directories("$ENV{NLOHMANN_JSON_ROOT}/include")
            # Create an imported target for nlohmann_json
            add_library(nlohmann_json::nlohmann_json INTERFACE IMPORTED)
            set_target_properties(nlohmann_json::nlohmann_json PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "$ENV{NLOHMANN_JSON_ROOT}/include"
            )
        else ()
            message(STATUS "nlohmann_json not found locally, fetching...")
            FetchContent_Declare(json
                    GIT_REPOSITORY https://github.com/nlohmann/json.git
                    GIT_TAG v3.10.5
                    GIT_SHALLOW ON
            )
            FetchContent_MakeAvailable(json)
        endif ()
    endif ()
endfunction()

# Fetch all dependencies
setup_ftxui()
setup_json()

# Define the include directories structure
set(TETRIS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(TETRIS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


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


# 1. Shared Common Library
file(GLOB COMMON_SRC_FILES "${TETRIS_SRC_DIR}/common/*.cpp")
add_library(TetrisRoyaleCommon ${COMMON_SRC_FILES})
target_include_directories(TetrisRoyaleCommon PUBLIC "${TETRIS_INCLUDE_DIR}/common")
set_target_compile_options(TetrisRoyaleCommon)

# 2. Server Common Library
file(GLOB SERVER_COMMON_SRC_FILES "${TETRIS_SRC_DIR}/common/server-common/*.cpp")
add_library(TetrisRoyaleCommonServer ${SERVER_COMMON_SRC_FILES})
target_include_directories(TetrisRoyaleCommonServer PUBLIC "${TETRIS_INCLUDE_DIR}/common/server-common")
target_link_libraries(TetrisRoyaleCommonServer PUBLIC TetrisRoyaleCommon Boost::boost OpenSSL::Crypto nlohmann_json::nlohmann_json)
set_target_compile_options(TetrisRoyaleCommonServer)

# 3. Game Logic Library
file(GLOB SERVER_LOGIC_SRC_FILES "${TETRIS_SRC_DIR}/server/server-game-logic/*.cpp")
add_library(TetrisRoyaleGameLogic STATIC ${SERVER_LOGIC_SRC_FILES})
target_include_directories(TetrisRoyaleGameLogic PUBLIC "${TETRIS_INCLUDE_DIR}/server/server-game-logic")
target_link_libraries(TetrisRoyaleGameLogic PUBLIC TetrisRoyaleCommon)
set_target_compile_options(TetrisRoyaleGameLogic)

# 4. HTTP Server Library
file(GLOB HTTP_SERVER_SRC_FILES "${TETRIS_SRC_DIR}/server/http-server/*.cpp")
add_library(TetrisRoyaleHTTPServer STATIC ${HTTP_SERVER_SRC_FILES})
target_include_directories(TetrisRoyaleHTTPServer PUBLIC "${TETRIS_INCLUDE_DIR}/server/http-server")
target_link_libraries(TetrisRoyaleHTTPServer PUBLIC Boost::boost)
set_target_compile_options(TetrisRoyaleHTTPServer)

# 5. DB Server Library
file(GLOB DB_SERVER_SRC_FILES "${TETRIS_SRC_DIR}/server/db-server/*.cpp")
add_library(TetrisRoyaleDBServer STATIC ${DB_SERVER_SRC_FILES})
target_include_directories(TetrisRoyaleDBServer PUBLIC "${TETRIS_INCLUDE_DIR}/server/db-server")
target_link_libraries(TetrisRoyaleDBServer PUBLIC TetrisRoyaleHTTPServer TetrisRoyaleCommonServer TetrisRoyaleCommon Boost::boost SQLite::SQLite3 OpenSSL::Crypto)
set_target_compile_options(TetrisRoyaleDBServer)

# 6. Tetris Server Library
file(GLOB TETRIS_SERVER_SRC_FILES "${TETRIS_SRC_DIR}/server/tetris-server/*.cpp")
add_library(TetrisRoyaleTetrisServer ${TETRIS_SERVER_SRC_FILES})
target_include_directories(TetrisRoyaleTetrisServer PUBLIC "${TETRIS_INCLUDE_DIR}/server/tetris-server")
target_link_libraries(TetrisRoyaleTetrisServer PUBLIC TetrisRoyaleGameLogic TetrisRoyaleCommonServer TetrisRoyaleCommon)
set_target_compile_options(TetrisRoyaleTetrisServer)

# 7. Master Server Executable
add_executable(TetrisRoyaleMasterServer "${TETRIS_SRC_DIR}/server/MasterServer.cpp")
target_include_directories(TetrisRoyaleMasterServer PUBLIC "${TETRIS_INCLUDE_DIR}/server")
target_link_libraries(TetrisRoyaleMasterServer PUBLIC TetrisRoyaleTetrisServer TetrisRoyaleDBServer TetrisRoyaleCommon)
set_target_compile_options(TetrisRoyaleMasterServer)

# 8. Client Connectivity Library
file(GLOB CLIENT_CONNECTIVITY_SRC_FILES "${TETRIS_SRC_DIR}/client/connectivity/*.cpp")
add_library(TetrisRoyaleClientConnectivity ${CLIENT_CONNECTIVITY_SRC_FILES})
target_include_directories(TetrisRoyaleClientConnectivity PUBLIC "${TETRIS_INCLUDE_DIR}/client/connectivity")
target_link_libraries(TetrisRoyaleClientConnectivity PUBLIC nlohmann_json::nlohmann_json TetrisRoyaleCommonServer TetrisRoyaleCommon Boost::boost)
set_target_compile_options(TetrisRoyaleClientConnectivity)

# 9. Client TUI Library
file(GLOB CLIENT_TUI_SRC_FILES "${TETRIS_SRC_DIR}/client/tui/*.cpp" "${TETRIS_SRC_DIR}/client/tui/menus/*.cpp")
add_library(TetrisRoyaleClientTUI ${CLIENT_TUI_SRC_FILES})
target_include_directories(TetrisRoyaleClientTUI PUBLIC "${TETRIS_INCLUDE_DIR}/client/tui" "${TETRIS_INCLUDE_DIR}/client/tui/menus")
target_link_libraries(TetrisRoyaleClientTUI PUBLIC TetrisRoyaleClientConnectivity ftxui::screen ftxui::dom ftxui::component)
set_target_compile_options(TetrisRoyaleClientTUI)

# 10. Client Executable
add_executable(TetrisRoyaleClient "${TETRIS_SRC_DIR}/client/Client.cpp")
target_include_directories(TetrisRoyaleClient PUBLIC "${TETRIS_INCLUDE_DIR}/client")
target_link_libraries(TetrisRoyaleClient PUBLIC TetrisRoyaleClientTUI TetrisRoyaleClientConnectivity)
set_target_compile_options(TetrisRoyaleClient)


# ====================================================
#                      Testing
# ====================================================
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

