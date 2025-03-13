#!/bin/bash
# Dependency installation script for TetrisRoyale
# This script installs all required dependencies locally

set -e # Exit on error

# We check for curl and unzip, as they are required for downloading and extracting dependencies
if ! command -v unzip > /dev/null; then
    echo "'unzip' is required but not installed. Please install it and try again."
    exit 1
fi

# Configuration
INSTALL_ROOT="$HOME/tetris-deps"
BOOST_VERSION="1.87.0"
NLOHMANN_JSON_VERSION="3.10.5"
SQLITE_VERSION="3420000"

# Create installation directory
mkdir -p "$INSTALL_ROOT"
mkdir -p "$INSTALL_ROOT/downloads"
cd "$INSTALL_ROOT/downloads"

# Set environment variables for the build
export BOOST_ROOT="$INSTALL_ROOT/boost"
export NLOHMANN_JSON_ROOT="$INSTALL_ROOT/json"
export SQLITE_ROOT="$INSTALL_ROOT/sqlite"
export CMAKE_PREFIX_PATH="$BOOST_ROOT:$NLOHMANN_JSON_ROOT:$SQLITE_ROOT:$CMAKE_PREFIX_PATH"

# Print status header function
print_header() {
    echo ""
    echo "=============================="
    echo "  $1"
    echo "=============================="
}

# ============================
# Install Boost
# ============================
if [ ! -d "$BOOST_ROOT/include/boost" ]; then
    print_header "Installing Boost $BOOST_VERSION"

    # Download Boost
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

    cd "$INSTALL_ROOT/downloads"
    echo "Boost installed successfully at ${BOOST_ROOT}"
else
    echo "Boost already installed at ${BOOST_ROOT}"
fi

# ============================
# Install nlohmann_json
# ============================
if [ ! -d "$NLOHMANN_JSON_ROOT/include/nlohmann" ]; then
    print_header "Installing nlohmann_json $NLOHMANN_JSON_VERSION"

    # Create installation directory
    mkdir -p "$NLOHMANN_JSON_ROOT/include"

    # Download and extract
    JSON_URL="https://github.com/nlohmann/json/releases/download/v${NLOHMANN_JSON_VERSION}/include.zip"
    JSON_ARCHIVE="nlohmann_json-${NLOHMANN_JSON_VERSION}.zip"

    if [ ! -f "$JSON_ARCHIVE" ]; then
        echo "Downloading nlohmann_json..."
        curl -L "${JSON_URL}" -o "$JSON_ARCHIVE"
    fi

    echo "Extracting nlohmann_json..."
    unzip -q -o "$JSON_ARCHIVE" -d "$NLOHMANN_JSON_ROOT/include"

    echo "nlohmann_json installed successfully at ${NLOHMANN_JSON_ROOT}"
else
    echo "nlohmann_json already installed at ${NLOHMANN_JSON_ROOT}"
fi

# ============================
# Install SQLite
# ============================
if [ ! -d "$SQLITE_ROOT/include" ]; then
    print_header "Installing SQLite $SQLITE_VERSION"

    # Create installation directory
    mkdir -p "$SQLITE_ROOT/include"
    mkdir -p "$SQLITE_ROOT/lib"

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

    echo "SQLite installed successfully at ${SQLITE_ROOT}"
else
    echo "SQLite already installed at ${SQLITE_ROOT}"
fi

# ============================
# Generate environment script
# ============================
print_header "Generating environment script"

ENV_SCRIPT="$INSTALL_ROOT/setup-env.sh"
cat > "$ENV_SCRIPT" << EOF
#!/bin/bash
# Environment setup for TetrisRoyale build

export BOOST_ROOT="$BOOST_ROOT"
export NLOHMANN_JSON_ROOT="$NLOHMANN_JSON_ROOT"
export SQLITE_ROOT="$SQLITE_ROOT"
export CMAKE_PREFIX_PATH="$BOOST_ROOT:$NLOHMANN_JSON_ROOT:$SQLITE_ROOT:\$CMAKE_PREFIX_PATH"

echo "Environment variables set for TetrisRoyale build"
echo "  BOOST_ROOT=$BOOST_ROOT"
echo "  NLOHMANN_JSON_ROOT=$NLOHMANN_JSON_ROOT"
echo "  SQLITE_ROOT=$SQLITE_ROOT"
EOF

chmod +x "$ENV_SCRIPT"
echo "Environment script created at $ENV_SCRIPT"
echo "Source this script before running CMake:"
echo "  source $ENV_SCRIPT"

print_header "All dependencies installed"
echo "You can now build TetrisRoyale with:"
echo "  source $ENV_SCRIPT"
echo "  mkdir -p build && cd build"
echo "  cmake .."
echo "  make"