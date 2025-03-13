#!/bin/bash
# Dependency installation script for TetrisRoyale

set -e # Exit on error

# We check for curl and unzip, as they are required for downloading and extracting dependencies
if ! command -v curl > /dev/null; then
    echo "'curl' is required but not installed. Please install it and try again."
    exit 1
fi

if ! command -v unzip > /dev/null; then
    echo "'unzip' is required but not installed. Please install it and try again."
    exit 1
fi

# Configuration
CURDIR=$(pwd)
INSTALL_ROOT="$CURDIR/lib"
BOOST_VERSION="1.87.0"
NLOHMANN_JSON_VERSION="3.10.5"
SQLITE_VERSION="3420000"

# Create installation directory if needed
mkdir -p "$INSTALL_ROOT/downloads"

# Print status header function
print_header() {
    echo ""
    echo "=============================="
    echo "  $1"
    echo "=============================="
}

# Function to check if a library exists in system paths
check_system_lib() {
    local lib_name="$1"
    local header_path="$2"
    local lib_file="$3"

    # Check if header exists
    if [ -f "/usr/include/$header_path" ] || [ -f "/usr/local/include/$header_path" ]; then
        # Check if library exists
        if [ -z "$lib_file" ] || ldconfig -p | grep -q "$lib_file"; then
            return 0  # Library found
        fi
    fi

    return 1  # Library not found
}

# ============================
# Setup Environment Variables
# ============================
USING_SYSTEM_BOOST=false
USING_SYSTEM_JSON=false
USING_SYSTEM_SQLITE=false

# ============================
# Check and Install Boost
# ============================
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

# ============================
# Check and Install nlohmann_json
# ============================
if check_system_lib "nlohmann_json" "nlohmann/json.hpp" ""; then
    print_header "Using system nlohmann_json"
    NLOHMANN_JSON_ROOT="/usr"
    USING_SYSTEM_JSON=true
elif [ -d "$INSTALL_ROOT/json/include/nlohmann" ]; then
    print_header "Using previously installed nlohmann_json"
    NLOHMANN_JSON_ROOT="$INSTALL_ROOT/json"
else
    print_header "Installing nlohmann_json $NLOHMANN_JSON_VERSION locally"
    NLOHMANN_JSON_ROOT="$INSTALL_ROOT/json"

    # Create installation directory
    mkdir -p "$NLOHMANN_JSON_ROOT/include"
    cd "$INSTALL_ROOT/downloads"

    # Download and extract
    JSON_URL="https://github.com/nlohmann/json/releases/download/v${NLOHMANN_JSON_VERSION}/include.zip"
    JSON_ARCHIVE="nlohmann_json-${NLOHMANN_JSON_VERSION}.zip"

    if [ ! -f "$JSON_ARCHIVE" ]; then
        echo "Downloading nlohmann_json..."
        curl -L "${JSON_URL}" -o "$JSON_ARCHIVE"
    fi

    echo "Extracting nlohmann_json..."
    unzip -q -o "$JSON_ARCHIVE" -d "$NLOHMANN_JSON_ROOT/include"

    cd "$CURDIR"
    echo "nlohmann_json installed successfully at ${NLOHMANN_JSON_ROOT}"
fi

# ============================
# Check and Install SQLite
# ============================
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

# ============================
# Generate environment script
# ============================
print_header "Generating environment script"

# Set CMAKE_PREFIX_PATH conditionally
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
# Remove trailing colon if exists
CMAKE_PREFIX_PATH=$(echo $CMAKE_PREFIX_PATH | sed 's/:$//')

ENV_SCRIPT="$INSTALL_ROOT/setup-env.sh"
cat > "$ENV_SCRIPT" << EOF
#!/bin/bash
# Environment setup for TetrisRoyale build

EOF

# Add variables only for non-system libraries
if ! $USING_SYSTEM_BOOST; then
    echo "export BOOST_ROOT=\"$BOOST_ROOT\"" >> "$ENV_SCRIPT"
fi
if ! $USING_SYSTEM_JSON; then
    echo "export NLOHMANN_JSON_ROOT=\"$NLOHMANN_JSON_ROOT\"" >> "$ENV_SCRIPT"
fi
if ! $USING_SYSTEM_SQLITE; then
    echo "export SQLITE_ROOT=\"$SQLITE_ROOT\"" >> "$ENV_SCRIPT"
fi
if [ ! -z "$CMAKE_PREFIX_PATH" ]; then
    echo "export CMAKE_PREFIX_PATH=\"$CMAKE_PREFIX_PATH:\$CMAKE_PREFIX_PATH\"" >> "$ENV_SCRIPT"
fi

cat >> "$ENV_SCRIPT" << EOF

echo "Environment variables set for TetrisRoyale build"
EOF

# Add echo statements only for non-system libraries
if ! $USING_SYSTEM_BOOST; then
    echo "echo \"  BOOST_ROOT=$BOOST_ROOT\"" >> "$ENV_SCRIPT"
else
    echo "echo \"  Using system Boost\"" >> "$ENV_SCRIPT"
fi
if ! $USING_SYSTEM_JSON; then
    echo "echo \"  NLOHMANN_JSON_ROOT=$NLOHMANN_JSON_ROOT\"" >> "$ENV_SCRIPT"
else
    echo "echo \"  Using system nlohmann_json\"" >> "$ENV_SCRIPT"
fi
if ! $USING_SYSTEM_SQLITE; then
    echo "echo \"  SQLITE_ROOT=$SQLITE_ROOT\"" >> "$ENV_SCRIPT"
else
    echo "echo \"  Using system SQLite\"" >> "$ENV_SCRIPT"
fi

chmod +x "$ENV_SCRIPT"
echo "Environment script created at $ENV_SCRIPT"
echo "Source this script before running CMake:"
echo "  source $ENV_SCRIPT"

print_header "Dependencies resolved"
echo "System libraries used:"
if $USING_SYSTEM_BOOST; then echo "  - Boost"; fi
if $USING_SYSTEM_JSON; then echo "  - nlohmann_json"; fi
if $USING_SYSTEM_SQLITE; then echo "  - SQLite"; fi

echo "Locally installed libraries:"
if ! $USING_SYSTEM_BOOST; then echo "  - Boost: $BOOST_ROOT"; fi
if ! $USING_SYSTEM_JSON; then echo "  - nlohmann_json: $NLOHMANN_JSON_ROOT"; fi
if ! $USING_SYSTEM_SQLITE; then echo "  - SQLite: $SQLITE_ROOT"; fi

echo "You can now build TetrisRoyale with:"
echo "  source $ENV_SCRIPT"
echo "  mkdir -p build && cd build"
echo "  cmake .."
echo "  make"