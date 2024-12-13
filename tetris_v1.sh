#!/bin/bash

# Navigate to the build directory
cd build || exit 1

# Call the build script (pass "clean" as an argument if you want to clean first)
./build.sh "$1"

# Run the executable
./run.sh