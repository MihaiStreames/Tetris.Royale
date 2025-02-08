#!/bin/bash

# Define directories and executable name
CMAKE_DIR="cmake"

# Optional clean step (unsure if to do like this or via cmake --clean-first)
if [ "$1" == "clean" ]; then
  echo "Cleaning build directory..."
  rm -rf "$CMAKE_DIR"
fi

# Create the CMake directory if it doesn't exist
mkdir -p "$CMAKE_DIR"
cd "$CMAKE_DIR" || exit 1

# Run CMake configuration and build
cmake ../..
cmake --build .

