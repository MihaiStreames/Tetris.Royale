#!/bin/bash


start_time=$(date +%s)

# TODO : fix the find_dependencies.sh script (somehow fails with zip)
echo "Running find_dependencies.sh..."
start_dep_time=$(date +%s)
sh ./build/find_dependencies.sh
 
if [ $? -ne 0 ]; then
    echo "find_dependencies.sh failed. Exiting."
    exit 1
fi
end_dep_time=$(date +%s)
dep_time=$((end_dep_time - start_dep_time))
echo "find_dependencies.sh completed in $dep_time seconds."

source "$HOME/tetris-deps/setup-env.sh"

echo "Creating test_build directory..."
mkdir -p test_build
cd test_build

echo "Running cmake .."
start_cmake_time=$(date +%s)
cmake ..

if [ $? -ne 0 ]; then
    echo "cmake failed. Exiting."
    exit 1
fi

end_cmake_time=$(date +%s)
cmake_time=$((end_cmake_time - start_cmake_time))
echo "cmake completed in $cmake_time seconds."


echo "Running make..."
start_make_time=$(date +%s)
make


if [ $? -ne 0 ]; then
    echo "make failed. Exiting."
    exit 1
fi

end_make_time=$(date +%s)
make_time=$((end_make_time - start_make_time))
echo "make completed in $make_time seconds."


end_time=$(date +%s)
total_time=$((end_time - start_time))

echo "Build completed successfully in $total_time seconds!"

