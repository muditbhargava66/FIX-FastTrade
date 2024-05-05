#!/bin/bash

# Set the project root directory
PROJECT_ROOT=$(dirname "$(dirname "$(dirname "$(readlink -f "$0")")")")

# Set the build directory
BUILD_DIR="$PROJECT_ROOT/build"

# Set the CMake generator (adjust as needed)
CMAKE_GENERATOR="Unix Makefiles"

# Set the build type (Release, Debug, etc.)
BUILD_TYPE="Release"

# Create the build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Change to the build directory
cd "$BUILD_DIR" || exit

# Configure the project with CMake
cmake -G "$CMAKE_GENERATOR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" "$PROJECT_ROOT"

# Build the project
cmake --build . --config "$BUILD_TYPE"

# Run the tests (if any)
ctest --output-on-failure