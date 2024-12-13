#!/bin/bash

# Define directories and executable name
CMAKE_DIR="cmake"
EXECUTABLE_NAME="TetrisRoyale_Terminal"
EXECUTABLE="$CMAKE_DIR/$EXECUTABLE_NAME"

# Check if the executable exists and run it
if [ -f "$EXECUTABLE" ]; then
  echo "Running $EXECUTABLE_NAME..."
  "$EXECUTABLE"
else
  echo "Error: Executable $EXECUTABLE not found."
  exit 1
fi