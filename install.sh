#!/bin/bash
# Enable error handling
set -e

# Configuration
BUILD_DIR="build"
DEPS_SCRIPT="./scripts/find_dependencies.sh"
DEPS_ENV_PATH="./lib/setup-env.sh"
CMAKE_ARGS=""
MAKE_ARGS="-j$(nproc)" # Use all available CPU cores by default
RUN_TESTS=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
  case $1 in
    --clean)
      CLEAN_BUILD=true
      shift
      ;;
    --debug)
      CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_BUILD_TYPE=Debug"
      shift
      ;;
    --release)
      CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release"
      shift
      ;;
    --enable-tests)
      CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release"
      RUN_TESTS=true
      shift
      ;;
    --jobs=*)
      MAKE_ARGS="-j${1#*=}"
      shift
      ;;
    --help)
      echo "Usage: $0 [options]"
      echo "Options:"
      echo "  --clean        Clean the build directory before building"
      echo "  --debug        Build in debug mode"
      echo "  --release      Build in release mode"
      echo "  --enable-tests Enable and run tests after building"
      echo "  --jobs=N       Use N jobs for make (default: all cores)"
      echo "  --help         Show this help message"
      exit 0
      ;;
    *)
      echo "Unknown option: $1"
      echo "Use --help for available options"
      exit 1
      ;;
  esac
done

# Function to print section headers
print_header() {
    local title="$1"
    local line=$(printf "%0.s=" $(seq 1 50))
    echo -e "\n$line"
    echo "  $title"
    echo "$line"
}

# Function to time a command and report results
run_timed_command() {
    local command_name="$1"
    local command="$2"
    local start_time=$(date +%s)

    print_header "Running $command_name"
    echo "$ $command"

    # Run the command and capture its exit status
    eval "$command"
    local status=$?

    local end_time=$(date +%s)
    local duration=$((end_time - start_time))

    echo "$command_name completed in $duration seconds."

    # Return the command's exit status
    return $status
}

# Start measuring total time
start_time=$(date +%s)

# Check if dependencies script exists
if [ ! -f "$DEPS_SCRIPT" ]; then
    echo "Error: Dependencies script not found at $DEPS_SCRIPT"
    exit 1
fi

# Make the dependencies script executable
chmod +x "$DEPS_SCRIPT"

# Run the dependencies finder script
run_timed_command "dependencies script" "$DEPS_SCRIPT"

# Check if environment script exists
if [ ! -f "$DEPS_ENV_PATH" ]; then
    echo "Error: Environment script not found at $DEPS_ENV_PATH"
    echo "The dependency script should have created this file."
    exit 1
fi

# Source the environment setup
echo "Sourcing environment from $DEPS_ENV_PATH"
source "$DEPS_ENV_PATH"

# run the environment setup script
if [ ! -f "$DEPS_ENV_PATH" ]; then
    echo "Error: Environment setup script not found at $DEPS_ENV_PATH"
    exit 1
fi
run_timed_command "environment setup" "source $DEPS_ENV_PATH"

# Clean build directory if requested
if [ "$CLEAN_BUILD" = true ]; then
    print_header "Cleaning build directory"
    if [ -d "$BUILD_DIR" ]; then
        echo "Removing existing $BUILD_DIR directory..."
        rm -rf "$BUILD_DIR"
    fi
fi

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating $BUILD_DIR directory..."
    mkdir -p "$BUILD_DIR"
fi

# Navigate to build directory
cd "$BUILD_DIR"

# Run CMake
run_timed_command "CMake configuration" "cmake $CMAKE_ARGS .."

# Run Make
run_timed_command "build process" "make $MAKE_ARGS"

if [ "$RUN_TESTS" = true ]; then
    print_header "Running tests"
    run_timed_command "test suite" "ctest --output-on-failure"
fi

# Run tests if they exist
if [ -f "ctest" ] || [ -f "make test" ]; then
    print_header "Running tests"
    if [ -f "ctest" ]; then
        run_timed_command "test suite" "ctest --output-on-failure"
    else
        run_timed_command "test suite" "make test"
    fi
fi

# Calculate and display total build time
end_time=$(date +%s)
total_time=$((end_time - start_time))

print_header "Build Summary"
echo "Build completed successfully in $total_time seconds!"
echo "You can run the application with:"
echo "  ./$BUILD_DIR/bin/TetrisRoyaleClient"

# Optional: List the binary files that were built
echo -e "\nBuilt executables:"
find . -maxdepth 1 -type f -executable -not -name "*.o" -not -name "*.a" | sort