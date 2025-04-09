#!/bin/bash

# dependency installation script for TetrisRoyale
set -e

# configuration (versions and paths)
CURDIR=$(pwd)
INSTALL_ROOT="$CURDIR/lib"
BOOST_VERSION="1.87.0"
NLOHMANN_JSON_VERSION="3.10.5"
SQLITE_VERSION="3420000"
OPENSSL_VERSION="3.5.0"
GTEST_VERSION="release-1.12.1"
FTXUI_VERSION="5.0.0"
mkdir -p "$INSTALL_ROOT/downloads"


# few functions to help with the installation process

print_header() {
    echo ""
    echo "=================================================="
    echo "$1"
    echo "=================================================="
}

check_system_lib() {

    local lib_name="$1"
    local header_path="$2"
    local lib_file="$3"

    # return 1  # debug to default all libs to not found -> local

    # check if header exists
    if [ -f "/usr/include/$header_path" ] || [ -f "/usr/local/include/$header_path" ]; then
        # check if library exists
        if [ -z "$lib_file" ] || ldconfig -p | grep -q "$lib_file"; then
            return 0  # library found
        fi
    fi

    return 1  # library not found

}

check_command() {
    local command="$1"
    if ! command -v "$command" > /dev/null; then
        echo "'$command' is required but not installed. Please install it and try again."
        exit 1
    fi
}


# we check for gcc, cmake, make, curl and unzip, as they are required for downloading, extracting and building dependencies
check_command "gcc"
check_command "cmake"
check_command "make"
check_command "curl"
check_command "unzip"


# ============================
# Setup Environment Variables
# ============================

USING_SYSTEM_BOOST=false
USING_SYSTEM_JSON=false
USING_SYSTEM_SQLITE=false
USING_SYSTEM_OPENSSL=false
USING_SYSTEM_GTEST=false
USING_SYSTEM_FTXUI=false

# ==================================
# Check and Install Boost
# ==================================

if check_system_lib "boost" "boost/version.hpp" "libboost_system"; then
    print_header "Using system Boost"
    BOOST_ROOT="/usr"
    USING_SYSTEM_BOOST=true
elif [ -d "$INSTALL_ROOT/boost/include/boost" ]; then
    print_header "Using previously installed Boost"
    BOOST_ROOT="$INSTALL_ROOT/boost"
else
    print_header "Installing Boost $BOOST_VERSION locally"
    BOOST_ROOT="$INSTALL_ROOT/boost"

    # Download Boost
    cd "$INSTALL_ROOT/downloads"
    BOOST_URL="https://github.com/boostorg/boost/releases/download/boost-${BOOST_VERSION}/boost-${BOOST_VERSION}-b2-nodocs.tar.gz"
    BOOST_ARCHIVE="boost-${BOOST_VERSION}-b2-nodocs.tar.gz"
    BOOST_DIR="boost-${BOOST_VERSION}"

    if [ ! -f "$BOOST_ARCHIVE" ]; then
        echo "Downloading Boost..."
        curl -L "${BOOST_URL}" -o "$BOOST_ARCHIVE"
    fi

    # Extract
    echo "Extracting Boost..."
    tar xf "$BOOST_ARCHIVE"

    # Navigate into the extracted folder
    cd "${BOOST_DIR}" || exit

    # Bootstrap with local installation path
    echo "Bootstrapping Boost..."
    ./bootstrap.sh --prefix="${BOOST_ROOT}"

    # Build and install locally
    echo "Building and installing Boost (this may take a while)..."
    ./b2 install --with-filesystem --with-system -j$(nproc)

    cd "$CURDIR"
    echo "Boost installed successfully at ${BOOST_ROOT}"
fi

# ==================================
# Check and Install nlohmann_json
# ==================================

if check_system_lib "nlohmann" "nlohmann/json.hpp" ""; then
    print_header "Using system nlohmann_json"
    NLOHMANN_JSON_ROOT="/usr"
    USING_SYSTEM_JSON=true
elif [ -d "$INSTALL_ROOT/nlohmann/include/nlohmann" ]; then
    print_header "Using previously installed nlohmann_json"
    NLOHMANN_JSON_ROOT="$INSTALL_ROOT/nlohmann"
else
    print_header "Installing nlohmann_json $NLOHMANN_JSON_VERSION locally"
    NLOHMANN_JSON_ROOT="$INSTALL_ROOT/nlohmann"

    # Create installation directory
    cd "$INSTALL_ROOT/downloads"

    # Download and extract
    JSON_URL="https://github.com/nlohmann/json/releases/download/v${NLOHMANN_JSON_VERSION}/include.zip"
    JSON_ARCHIVE="nlohmann_json-${NLOHMANN_JSON_VERSION}.zip"

    if [ ! -f "$JSON_ARCHIVE" ]; then
        echo "Downloading nlohmann_json..."
        curl -L "${JSON_URL}" -o "$JSON_ARCHIVE"
    fi

    echo "Extracting nlohmann_json..."
    unzip -q -o "$JSON_ARCHIVE" -d "$NLOHMANN_JSON_ROOT"

    cd "$CURDIR"
    echo "nlohmann_json installed successfully at ${NLOHMANN_JSON_ROOT}"
fi

# ==================================
# Check and Install SQLite
# ==================================

if check_system_lib "sqlite3" "sqlite3.h" "libsqlite3.so"; then
    print_header "Using system SQLite"
    SQLITE_ROOT="/usr"
    USING_SYSTEM_SQLITE=true
elif [ -d "$INSTALL_ROOT/sqlite/include" ] && [ -f "$INSTALL_ROOT/sqlite/include/sqlite3.h" ]; then
    print_header "Using previously installed SQLite"
    SQLITE_ROOT="$INSTALL_ROOT/sqlite"
else
    print_header "Installing SQLite $SQLITE_VERSION locally"
    SQLITE_ROOT="$INSTALL_ROOT/sqlite"

    # Create installation directory
    mkdir -p "$SQLITE_ROOT/include"
    mkdir -p "$SQLITE_ROOT/lib"
    cd "$INSTALL_ROOT/downloads"

    # Download and extract
    SQLITE_URL="https://sqlite.org/2023/sqlite-amalgamation-${SQLITE_VERSION}.zip"
    SQLITE_ARCHIVE="sqlite-${SQLITE_VERSION}.zip"

    if [ ! -f "$SQLITE_ARCHIVE" ]; then
        echo "Downloading SQLite..."
        curl -L "${SQLITE_URL}" -o "$SQLITE_ARCHIVE"
    fi

    echo "Extracting SQLite..."
    unzip -q -o "$SQLITE_ARCHIVE" -d "$INSTALL_ROOT/downloads"

    # Verify the source file exists
    SQLITE_SRC_DIR="$INSTALL_ROOT/downloads/sqlite-amalgamation-${SQLITE_VERSION}"
    if [ ! -f "$SQLITE_SRC_DIR/sqlite3.c" ]; then
        echo "Error: sqlite3.c not found in the extracted archive."
        exit 1
    fi

    # Copy header file
    cp "$SQLITE_SRC_DIR/sqlite3.h" "$SQLITE_ROOT/include/"

    # Compile SQLite
    echo "Compiling SQLite..."
    gcc -c "$SQLITE_SRC_DIR/sqlite3.c" -o "$INSTALL_ROOT/downloads/sqlite3.o" -O2 -fPIC
    if [ $? -ne 0 ]; then
        echo "Error: Compilation of sqlite3.c failed."
        exit 1
    fi

    # Create shared library
    gcc -shared -o "$SQLITE_ROOT/lib/libsqlite3.so" "$INSTALL_ROOT/downloads/sqlite3.o" -ldl -lpthread
    if [ $? -ne 0 ]; then
        echo "Error: Failed to create shared library libsqlite3.so."
        exit 1
    fi

    # Create static library
    ar rcs "$SQLITE_ROOT/lib/libsqlite3.a" "$INSTALL_ROOT/downloads/sqlite3.o"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to create static library libsqlite3.a."
        exit 1
    fi

    cd "$CURDIR"
    echo "SQLite installed successfully at ${SQLITE_ROOT}"
fi

# ==================================
# Check and Install OpenSSL
# ==================================

if check_system_lib "openssl" "openssl/ssl.h" "libssl.so"; then
    print_header "Using system OpenSSL"
    OPENSSL_ROOT="/usr"
    USING_SYSTEM_OPENSSL=true
elif [ -d "$INSTALL_ROOT/openssl" ]; then
    print_header "Using previously installed OpenSSL"
    OPENSSL_ROOT="$INSTALL_ROOT/openssl"
else
    print_header "Installing OpenSSL $OPENSSL_VERSION locally"
    OPENSSL_ROOT="$INSTALL_ROOT/openssl"
    cd "$INSTALL_ROOT/downloads"

    OPENSSL_TAR="openssl-${OPENSSL_VERSION}.tar.gz"
    OPENSSL_URL="https://www.openssl.org/source/$OPENSSL_TAR"

    if [ ! -f "$OPENSSL_TAR" ]; then
        echo "Downloading OpenSSL..."
        curl -LO "$OPENSSL_URL"
    fi

    echo "Extracting OpenSSL..."
    tar xf "$OPENSSL_TAR"
    cd "openssl-${OPENSSL_VERSION}"

    echo "Configuring OpenSSL..."
    ./config --prefix="$OPENSSL_ROOT" no-shared

    echo "Building OpenSSL..."
    make -j$(nproc)
    make install_sw

    cd "$CURDIR"
    echo "OpenSSL installed successfully at $OPENSSL_ROOT"
fi

# ==================================
# Check and Install GoogleTest
# ==================================

if check_system_lib "gtest" "gtest/gtest.h" "libgtest.a"; then
    print_header "Using system GoogleTest"
    GTEST_ROOT="/usr"
    USING_SYSTEM_GTEST=true
elif [ -d "$INSTALL_ROOT/googletest" ]; then
    print_header "Using previously installed GoogleTest"
    GTEST_ROOT="$INSTALL_ROOT/googletest"
else
    print_header "Installing GoogleTest locally"
    GTEST_ROOT="$INSTALL_ROOT/googletest"

    # Create installation directory
    mkdir -p "$GTEST_ROOT"
    cd "$INSTALL_ROOT/downloads"

    # Download and extract GoogleTest
    GTEST_URL="https://github.com/google/googletest/archive/${GTEST_VERSION}.zip"
    GTEST_ARCHIVE="googletest-${GTEST_VERSION}.zip"

    if [ ! -f "$GTEST_ARCHIVE" ]; then
        echo "Downloading GoogleTest..."
        curl -L "${GTEST_URL}" -o "$GTEST_ARCHIVE"
    fi

    echo "Extracting GoogleTest..."
    unzip -q -o "$GTEST_ARCHIVE" -d "$INSTALL_ROOT/downloads"

    # Navigate into the extracted folder
    GTEST_SRC_DIR="$INSTALL_ROOT/downloads/googletest-${GTEST_VERSION}"
    cd "$GTEST_SRC_DIR" || exit

    # Create and enter the build directory
    mkdir -p build
    cd build

    # Run CMake to configure the build
    echo "Configuring GoogleTest..."
    cmake .. -DCMAKE_INSTALL_PREFIX="$GTEST_ROOT"

    # Build and install GoogleTest
    echo "Building and installing GoogleTest (this may take a while)..."
    make -j$(nproc)
    make install

    cd "$CURDIR"
    echo "GoogleTest installed successfully at ${GTEST_ROOT}"
fi

# ==================================
# Check and Install FTXUI
# ==================================

if check_system_lib "ftxui" "ftxui/component/component.hpp" ""; then
    print_header "Using system FTXUI"
    FTXUI_ROOT="/usr"
    USING_SYSTEM_FTXUI=true
elif [ -d "$INSTALL_ROOT/ftxui/include/ftxui" ]; then
    print_header "Using previously installed FTXUI"
    FTXUI_ROOT="$INSTALL_ROOT/ftxui"
else
    print_header "Installing FTXUI $FTXUI_VERSION locally"
    FTXUI_ROOT="$INSTALL_ROOT/ftxui"
    mkdir -p "$FTXUI_ROOT"

    # Create installation directory
    cd "$INSTALL_ROOT/downloads"

    # Download and extract FTXUI
    FTXUI_URL="https://github.com/ArthurSonzogni/FTXUI/releases/download/v${FTXUI_VERSION}/ftxui-${FTXUI_VERSION}-Linux.tar.gz"
    FTXUI_ARCHIVE="ftxui-${FTXUI_VERSION}-Linux.tar.gz"

    if [ ! -f "$FTXUI_ARCHIVE" ]; then
        echo "Downloading FTXUI..."
        curl -L "${FTXUI_URL}" -o "$FTXUI_ARCHIVE"
    fi

    echo "Extracting FTXUI..."
    tar -xzf "$FTXUI_ARCHIVE" -C "$FTXUI_ROOT"

    mv "$INSTALL_ROOT/ftxui/ftxui-${FTXUI_VERSION}-Linux/include" "$INSTALL_ROOT/ftxui/include"
    mv "$INSTALL_ROOT/ftxui/ftxui-${FTXUI_VERSION}-Linux/lib" "$INSTALL_ROOT/ftxui/lib"

    # Remove the temporary folder that was created during extraction
    rm -rf "$INSTALL_ROOT/ftxui/ftxui-${FTXUI_VERSION}-Linux"

    cd "$CURDIR"
    echo "FTXUI installed successfully at ${FTXUI_ROOT}"
fi

# ==================================
# Generate environment script
# ==================================

print_header "Generating environment script"

# set CMAKE_PREFIX_PATH if not using system libraries
CMAKE_PREFIX_PATH=""

if ! $USING_SYSTEM_BOOST; then
    CMAKE_PREFIX_PATH="${BOOST_ROOT}:${CMAKE_PREFIX_PATH}"
fi
if ! $USING_SYSTEM_JSON; then
    CMAKE_PREFIX_PATH="${NLOHMANN_JSON_ROOT}:${CMAKE_PREFIX_PATH}"
fi
if ! $USING_SYSTEM_SQLITE; then
    CMAKE_PREFIX_PATH="${SQLITE_ROOT}:${CMAKE_PREFIX_PATH}"
fi
if ! $USING_SYSTEM_OPENSSL; then
    CMAKE_PREFIX_PATH="${OPENSSL_ROOT}:${CMAKE_PREFIX_PATH}"
fi
if ! $USING_SYSTEM_GTEST; then
    CMAKE_PREFIX_PATH="${GTEST_ROOT}:${CMAKE_PREFIX_PATH}"
fi
if ! $USING_SYSTEM_FTXUI; then
    CMAKE_PREFIX_PATH="${FTXUI_ROOT}:${CMAKE_PREFIX_PATH}"
fi

# remove trailing colon if exists (I had a fucking bug with this once and it took me a while to figure out)
CMAKE_PREFIX_PATH=$(echo $CMAKE_PREFIX_PATH | sed 's/:$//')


# create the environment script and make it executable
ENV_SCRIPT="$INSTALL_ROOT/setup-env.sh"
{
    echo "#!/bin/bash"
    echo "# Environment setup for TetrisRoyale build"

    # export variables for non-system libraries if needed
    [ "$USING_SYSTEM_BOOST" = false ] && echo "export BOOST_ROOT=\"$BOOST_ROOT\""
    [ "$USING_SYSTEM_JSON" = false ] && echo "export NLOHMANN_JSON_ROOT=\"$NLOHMANN_JSON_ROOT\""
    [ "$USING_SYSTEM_SQLITE" = false ] && echo "export SQLITE_ROOT=\"$SQLITE_ROOT\""
    [ "$USING_SYSTEM_OPENSSL" = false ] && echo "export OPENSSL_ROOT=\"$OPENSSL_ROOT\""
    [ "$USING_SYSTEM_GTEST" = false ] && echo "export GTEST_ROOT=\"$GTEST_ROOT\""
    [ "$USING_SYSTEM_FTXUI" = false ] && echo "export FTXUI_ROOT=\"$FTXUI_ROOT\""

    # add CMAKE_PREFIX_PATH if it's set
    if [ -n "$CMAKE_PREFIX_PATH" ]; then
        echo "export CMAKE_PREFIX_PATH=\"$CMAKE_PREFIX_PATH:\$CMAKE_PREFIX_PATH\""
    fi

    # confirmation message
    echo 'echo "Environment variables set for TetrisRoyale build"'

} > "$ENV_SCRIPT"
chmod +x "$ENV_SCRIPT"

# print the environment script path
echo "Environment script created at $ENV_SCRIPT"
echo "Source this script before running CMake:"
echo "  source $ENV_SCRIPT"

# some cool looking output to show the user what libraries were used
print_header "Dependencies resolved"
echo "System libraries used:"
if $USING_SYSTEM_BOOST; then echo "  - Boost"; fi
if $USING_SYSTEM_JSON; then echo "  - nlohmann_json"; fi
if $USING_SYSTEM_SQLITE; then echo "  - SQLite"; fi
if $USING_SYSTEM_OPENSSL; then echo "  - OpenSSL"; fi
if $USING_SYSTEM_GTEST; then echo "  - GoogleTest"; fi
if $USING_SYSTEM_FTXUI; then echo "  - FTXUI"; fi
echo "Locally installed libraries:"
if ! $USING_SYSTEM_BOOST; then echo "  - Boost: $BOOST_ROOT"; fi
if ! $USING_SYSTEM_JSON; then echo "  - nlohmann_json: $NLOHMANN_JSON_ROOT"; fi
if ! $USING_SYSTEM_SQLITE; then echo "  - SQLite: $SQLITE_ROOT"; fi
if ! $USING_SYSTEM_OPENSSL; then echo "  - OpenSSL: $OPENSSL_ROOT"; fi
if ! $USING_SYSTEM_GTEST; then echo "  - GoogleTest: $GTEST_ROOT"; fi
if ! $USING_SYSTEM_FTXUI; then echo "  - FTXUI: $FTXUI_ROOT"; fi


# end of the script
exit 0
