#!/bin/bash

# Cross-platform CPU core detection (Linux/macOS)
# Outputs the number of logical CPU cores, fallback to 1 if unknown

cpu_count=""

if command -v nproc > /dev/null 2>&1; then
    cpu_count=$(nproc)
elif [[ "$OSTYPE" == "darwin"* ]]; then
    cpu_count=$(sysctl -n hw.ncpu)
fi

if [[ -z "$cpu_count" || "$cpu_count" -lt 1 ]]; then
    cpu_count=1  # fallback if detection fails
fi

echo "$cpu_count"
