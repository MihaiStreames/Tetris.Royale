#!/bin/bash
# enable error handling
set -e


# function to print section headers
print_header() {
    local title="$1"
    local line=$(printf "%0.s=" $(seq 1 50))
    echo -e "\n$line"
    echo "  $title"
    echo "$line"
}

# function to time a command and report results
run_timed_command() {

    local command_name="$1"
    local command="$2"
    local start_time=$(date +%s)

    print_header "Running $command_name"
    echo "$ $command"

    eval "$command"
    local status=$?
    local end_time=$(date +%s)
    local duration=$((end_time - start_time))

    echo "$command_name completed in $duration seconds."
    return $status
}


# check if the script is run from the correct directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: This script must be run from the root directory of the project."
    exit 1
fi

# check if the script 'scripts/get_nproc.sh' exists
if [ ! -f "scripts/get_nproc.sh" ]; then
    echo "Error: The script 'scripts/get_nproc.sh' is missing."
    exit 1
fi

# make the script executable
chmod +x scripts/get_nproc.sh


# some variables
BUILD_DIR="build"
DEPENDENCIES_SCRIPT="./scripts/find_dependencies.sh"
ENV_SETUP_SCRIPT="./lib/setup-env.sh"
CMAKE_ARGS=""
MAKE_ARGS="-j$(./scripts/get_nproc.sh)" # use all available CPU cores by default
RUN_TESTS=false


# parse command line arguments
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

# start measuring total time
start_time=$(date +%s)

# check if dependencies script exists and then
# make the dependencies script executable to finally run it
if [ ! -f "$DEPENDENCIES_SCRIPT" ]; then
    echo "Error: Dependencies script not found at $DEPENDENCIES_SCRIPT"
    exit 1
fi
chmod +x "$DEPENDENCIES_SCRIPT"
run_timed_command "dependencies script" "$DEPENDENCIES_SCRIPT"

# check if the environment setup script exists and then run it
if [ ! -f "$ENV_SETUP_SCRIPT" ]; then
    echo "Error: Environment script not found at $ENV_SETUP_SCRIPT"
    echo "The dependency script should have created this file."
    exit 1
fi
source "$ENV_SETUP_SCRIPT"

# print the potentially set environment variables
print_header "Environment PATH:"
echo ${CMAKE_PREFIX_PATH}

# clean build directory if requested
if [ "$CLEAN_BUILD" = true ]; then
    print_header "Cleaning build directory"
    if [ -d "$BUILD_DIR" ]; then
        echo "Removing existing $BUILD_DIR directory..."
        rm -rf "$BUILD_DIR"
    fi
fi

# create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating $BUILD_DIR directory..."
    mkdir -p "$BUILD_DIR"
fi

# navigate to build directory
cd "$BUILD_DIR"

# then finally run CMake and make
run_timed_command "CMake configuration" "cmake $CMAKE_ARGS .."
run_timed_command "build process" "make $MAKE_ARGS"

# clean the lib/downloads directory if it exists
if [ -d "../lib/downloads" ]; then
    print_header "Cleaning downloads directory"
    echo "Removing existing lib/downloads directory..."
    rm -rf ../lib/downloads
fi

# and finally run tests if requested
if [ "$RUN_TESTS" = true ]; then
    print_header "Running tests"
    run_timed_command "test suite" "ctest --output-on-failure"
fi


# calculate and display total build time
end_time=$(date +%s)
total_time=$((end_time - start_time))

# print summary and executable, and then exit
print_header "Build Summary"
echo "Build completed successfully in $total_time seconds!"
echo "You can run the application with:"
echo "  ./$BUILD_DIR/bin/TetrisRoyaleClientTUI"
echo "  ./$BUILD_DIR/bin/TetrisRoyaleMasterServer"
echo -e "\nBuilt executables:"
find . -maxdepth 1 -type f -executable -not -name "*.o" -not -name "*.a" | sort

exit 0
