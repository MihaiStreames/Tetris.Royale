#!/bin/bash

# Variables
BOOST_URL="https://github.com/boostorg/boost/releases/download/boost-1.87.0/boost-1.87.0-b2-nodocs.tar.gz"
BOOST_DIR="boost-1.87.0"
INSTALL_DIR="$HOME/boost"

# Download Boost
wget -L "${BOOST_URL}"

# Extract
tar xvf boost-1.87.0-b2-nodocs.tar.gz

# Navigate into the extracted folder
cd "${BOOST_DIR}" || exit

# Bootstrap with local installation path
./bootstrap.sh --prefix="${INSTALL_DIR}"

# Build and install locally
./b2 install

# Export Boost path for CMake
export BOOST_ROOT="${INSTALL_DIR}"
export CMAKE_PREFIX_PATH="${BOOST_ROOT}:${CMAKE_PREFIX_PATH}"

echo "Boost installed successfully at ${BOOST_ROOT}"

