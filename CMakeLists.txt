cmake_minimum_required(VERSION 3.22)

# This is a revised version of Mihai's CMake template,
# rewritten by Antoine & Mihai to include boost stuff

# Project name and standard
project(TetrisRoyale VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(TETRIS_ENABLE_TESTS "Enable tests" OFF)
option(TETRIS_USE_SANITIZERS "Enable sanitizers in debug mode" OFF)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set policies (yeah well I don't like the new behavior & the warnings lol)
# cmake_policy(SET CMP0167 NEW)
# ok so for some reason this policy is not available in 3.22, so we'll just ignore it

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

# Dependencies management - prioritize locally installed packages
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# Find OpenSSL - required, fail if not found
find_package(OpenSSL REQUIRED)
message(STATUS "Found OpenSSL: ${OPENSSL_VERSION}")

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

# Setup Boost
function(setup_boost)
    # Try to find through standard methods first
    find_package(Boost 1.78.0 COMPONENTS filesystem system QUIET)

    if (Boost_FOUND)
        message(STATUS "Found Boost: ${Boost_VERSION}")
    else ()
        # Check for environment variables from your script
        if (DEFINED ENV{BOOST_ROOT} AND EXISTS "$ENV{BOOST_ROOT}/include/boost")
            message(STATUS "Using locally installed Boost from $ENV{BOOST_ROOT}")
            set(BOOST_ROOT $ENV{BOOST_ROOT} PARENT_SCOPE)
            set(Boost_INCLUDE_DIRS "$ENV{BOOST_ROOT}/include" PARENT_SCOPE)
            set(Boost_LIBRARY_DIRS "$ENV{BOOST_ROOT}/lib" PARENT_SCOPE)
            include_directories("$ENV{BOOST_ROOT}/include")
            link_directories("$ENV{BOOST_ROOT}/lib")
            set(Boost_FOUND TRUE PARENT_SCOPE)
        else ()
            message(STATUS "Boost not found locally. Please run the installation script first.")
            message(FATAL_ERROR "Boost is required but not found. Run your installation script before continuing.")
            # We won't fetch Boost automatically as it's very large and your script handles it
        endif ()
    endif ()
endfunction()

# Setup SQLite
function(setup_sqlite)
    find_package(SQLite3 QUIET)
    if (SQLite3_FOUND)
        message(STATUS "Found SQLite3: ${SQLite3_VERSION}")
    else ()
        # Check for environment variable pointing to local installation
        if (DEFINED ENV{SQLITE_ROOT} AND EXISTS "$ENV{SQLITE_ROOT}/include/sqlite3.h")
            message(STATUS "Using locally installed SQLite from $ENV{SQLITE_ROOT}")
            include_directories("$ENV{SQLITE_ROOT}/include")
            link_directories("$ENV{SQLITE_ROOT}/lib")
            # Create an imported target for SQLite
            add_library(sqlite3 INTERFACE IMPORTED)
            set_target_properties(sqlite3 PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "$ENV{SQLITE_ROOT}/include"
            )
        else ()
            message(STATUS "SQLite3 not found locally, using system libraries or fetching...")
            # Try to find sqlite3 in standard system locations
            find_library(SQLITE3_LIBRARY sqlite3)
            if (NOT SQLITE3_LIBRARY)
                message(STATUS "SQLite3 not found in system locations, fetching...")
                FetchContent_Declare(sqlite
                        URL https://sqlite.org/2023/sqlite-amalgamation-3420000.zip
                        URL_HASH SHA256=73038b2c829229e9b4e37a739c70e8beddd3c2f0673830a74effb1f4e8d3211e
                )
                FetchContent_MakeAvailable(sqlite)

                # Create SQLite3 library
                add_library(sqlite3 STATIC
                        ${sqlite_SOURCE_DIR}/sqlite3.c
                )
                target_include_directories(sqlite3 PUBLIC ${sqlite_SOURCE_DIR})
            endif ()
        endif ()
        set(SQLite3_FOUND TRUE PARENT_SCOPE)
    endif ()
endfunction()

# Fetch all dependencies
setup_ftxui()
setup_json()
setup_boost()
setup_sqlite()

# Define the include directories structure
set(TETRIS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(TETRIS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add the FTXUI include directories for local development
include_directories(${TETRIS_INCLUDE_DIR})
include_directories(${TETRIS_INCLUDE_DIR}/client/tui)
include_directories(${TETRIS_INCLUDE_DIR}/client/tui/menus)
include_directories(${TETRIS_INCLUDE_DIR}/client/connectivity)
include_directories(${TETRIS_INCLUDE_DIR}/common)
include_directories(${TETRIS_INCLUDE_DIR}/common/server-common)
include_directories(${TETRIS_INCLUDE_DIR}/server/server-game-logic)

# Build components
# ----------------------------------------------------------

# 1. Server Common Library
file(GLOB SERVER_COMMON_SRC_FILES
        "${TETRIS_SRC_DIR}/common/server-common/*.cpp"
)
add_library(TetrisRoyaleCommonServer ${SERVER_COMMON_SRC_FILES})
target_include_directories(TetrisRoyaleCommonServer
        PUBLIC
        "${TETRIS_INCLUDE_DIR}/common"
        "${TETRIS_INCLUDE_DIR}/common/server-common"
)
target_link_libraries(TetrisRoyaleCommonServer
        PUBLIC
        nlohmann_json::nlohmann_json
        PRIVATE
        OpenSSL::Crypto
        OpenSSL::SSL
)
set_target_compile_options(TetrisRoyaleCommonServer)
target_compile_definitions(TetrisRoyaleCommonServer PRIVATE BOOST_UUID_HEADER_ONLY)

# 2. Shared Common Library
file(GLOB COMMON_SRC_FILES
        "${TETRIS_SRC_DIR}/common/*.cpp"
)
add_library(TetrisRoyaleCommon STATIC ${COMMON_SRC_FILES})
target_include_directories(TetrisRoyaleCommon
        PUBLIC
        "${TETRIS_INCLUDE_DIR}/common"
        "${TETRIS_INCLUDE_DIR}/common/server-common"
)
target_link_libraries(TetrisRoyaleCommon
        PUBLIC
        TetrisRoyaleCommonServer
        nlohmann_json::nlohmann_json
)
set_target_compile_options(TetrisRoyaleCommon)

# 3. Server Game Logic Library
file(GLOB SERVER_LOGIC_SRC_FILES
        "${TETRIS_SRC_DIR}/server/server-game-logic/*.cpp"
)
add_library(TetrisRoyaleServerLogic STATIC ${SERVER_LOGIC_SRC_FILES})
target_include_directories(TetrisRoyaleServerLogic
        PUBLIC
        "${TETRIS_INCLUDE_DIR}/server"
        "${TETRIS_INCLUDE_DIR}/server/server-game-logic"
)
target_link_libraries(TetrisRoyaleServerLogic
        PUBLIC
        TetrisRoyaleCommon
)
set_target_compile_options(TetrisRoyaleServerLogic)

# 4. HTTP Server Library
file(GLOB HTTP_SERVER_SRC_FILES "${TETRIS_SRC_DIR}/server/http-server/*.cpp")
add_library(TetrisRoyaleHTTP STATIC ${HTTP_SERVER_SRC_FILES})
target_include_directories(TetrisRoyaleHTTP
        PUBLIC
        "${TETRIS_INCLUDE_DIR}/server/http-server"
)
set_target_compile_options(TetrisRoyaleHTTP)

# 5. DB Server Library
file(GLOB DB_SERVER_SRC_FILES "${TETRIS_SRC_DIR}/server/db-server/*.cpp")
add_library(TetrisRoyaleDBServer STATIC ${DB_SERVER_SRC_FILES})
target_include_directories(TetrisRoyaleDBServer
        PUBLIC
        "${TETRIS_INCLUDE_DIR}/server/db-server"
)
target_link_libraries(TetrisRoyaleDBServer
        PUBLIC
        TetrisRoyaleCommonServer
        TetrisRoyaleHTTP
        PRIVATE
        sqlite3
        OpenSSL::Crypto
)
set_target_compile_options(TetrisRoyaleDBServer)
target_compile_definitions(TetrisRoyaleDBServer PRIVATE BOOST_UUID_HEADER_ONLY)

# 6. Master Server Executable
file(GLOB TETRIS_MASTER_SERVER_SRC_FILES
        "${TETRIS_SRC_DIR}/server/tetris-server/*.cpp"
)
add_executable(TetrisRoyaleMasterServer
        "${TETRIS_SRC_DIR}/server/MasterServer.cpp"
        ${TETRIS_MASTER_SERVER_SRC_FILES})
target_include_directories(TetrisRoyaleMasterServer
        PRIVATE
        "${TETRIS_INCLUDE_DIR}/server"
        "${TETRIS_INCLUDE_DIR}/server/db-server"
        "${TETRIS_INCLUDE_DIR}/server/http-server"
        "${TETRIS_INCLUDE_DIR}/server/tetris-server"
        "${TETRIS_INCLUDE_DIR}/server/server-game-logic"
)
target_link_libraries(TetrisRoyaleMasterServer
        PRIVATE
        TetrisRoyaleCommonServer
        TetrisRoyaleServerLogic
        TetrisRoyaleHTTP
        TetrisRoyaleDBServer
        sqlite3
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
)
set_target_compile_options(TetrisRoyaleMasterServer)

# 7. Client Executable
file(GLOB TETRIS_CLIENT_SRC_FILES
        "${TETRIS_SRC_DIR}/client/*.cpp"
        "${TETRIS_SRC_DIR}/client/connectivity/*.cpp"
        "${TETRIS_SRC_DIR}/client/tui/*.cpp"
        "${TETRIS_SRC_DIR}/client/tui/menus/*.cpp"
)
add_executable(TetrisRoyaleClient
        "${TETRIS_SRC_DIR}/client/Client.cpp"
        ${TETRIS_CLIENT_SRC_FILES})
target_include_directories(TetrisRoyaleClient
        PRIVATE
        "${TETRIS_INCLUDE_DIR}/client"
        "${TETRIS_INCLUDE_DIR}/client/tui"
        "${TETRIS_INCLUDE_DIR}/client/tui/menus"
        "${TETRIS_INCLUDE_DIR}/client/connectivity"
        "${TETRIS_INCLUDE_DIR}/common"
        "${TETRIS_INCLUDE_DIR}/common/server-common"
        "${TETRIS_INCLUDE_DIR}/server/server-game-logic"
)
target_link_libraries(TetrisRoyaleClient
        PRIVATE
        TetrisRoyaleCommonServer
        TetrisRoyaleCommon
        TetrisRoyaleServerLogic
        nlohmann_json::nlohmann_json
        ftxui::screen
        ftxui::dom
        ftxui::component
)
set_target_compile_options(TetrisRoyaleClient)
target_compile_definitions(TetrisRoyaleClient PRIVATE
        BOOST_ASIO_HEADER_ONLY
        BOOST_BEAST_HEADER_ONLY
)

# Install targets
install(TARGETS
        TetrisRoyaleMasterServer
        TetrisRoyaleClient
        DESTINATION bin
)

# Optionally enable testing
if (TETRIS_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

# Print configuration summary
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "Testing enabled: ${TETRIS_ENABLE_TESTS}")
message(STATUS "Sanitizers enabled: ${TETRIS_USE_SANITIZERS}")