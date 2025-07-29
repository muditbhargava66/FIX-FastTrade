#!/bin/bash

# FIX-FastTrade Build Script
# This script builds the FIX-FastTrade project with proper error handling

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to get number of CPU cores (cross-platform)
get_cpu_cores() {
    if command -v nproc &> /dev/null; then
        nproc
    elif command -v sysctl &> /dev/null; then
        sysctl -n hw.ncpu
    else
        echo 4  # fallback
    fi
}

# Detect OS and set appropriate settings
OS="unknown"
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    OS="windows"
fi

# Set the project root directory (handle different readlink implementations)
if command -v readlink &> /dev/null && readlink -f "$0" &> /dev/null; then
    PROJECT_ROOT=$(dirname "$(dirname "$(dirname "$(readlink -f "$0")")")")
else
    # Fallback for macOS and other systems without GNU readlink
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
fi

print_status "Project root: $PROJECT_ROOT"
print_status "Detected OS: $OS"

# Verify we're in the right directory
if [[ ! -f "$PROJECT_ROOT/CMakeLists.txt" ]]; then
    print_error "CMakeLists.txt not found in project root: $PROJECT_ROOT"
    print_error "Please ensure this script is in the correct location"
    exit 1
fi

# Set the build directory
BUILD_DIR="$PROJECT_ROOT/build"

# Parse command line arguments
BUILD_TYPE="Release"
CMAKE_GENERATOR="Unix Makefiles"
PARALLEL_JOBS=$(get_cpu_cores)
RUN_TESTS=false
CLEAN_BUILD=false
VERBOSE=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -r|--release)
            BUILD_TYPE="Release"
            shift
            ;;
        -j|--jobs)
            PARALLEL_JOBS="$2"
            shift 2
            ;;
        -t|--test)
            RUN_TESTS=true
            shift
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -g|--generator)
            CMAKE_GENERATOR="$2"
            shift 2
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  -d, --debug       Build in Debug mode"
            echo "  -r, --release     Build in Release mode (default)"
            echo "  -j, --jobs N      Use N parallel jobs (default: auto-detect)"
            echo "  -t, --test        Run tests after building"
            echo "  -c, --clean       Clean build directory before building"
            echo "  -v, --verbose     Verbose build output"
            echo "  -g, --generator   CMake generator (default: Unix Makefiles)"
            echo "  -h, --help        Show this help message"
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            exit 1
            ;;
    esac
done

print_status "Build configuration:"
print_status "  Build Type: $BUILD_TYPE"
print_status "  Generator: $CMAKE_GENERATOR"
print_status "  Parallel Jobs: $PARALLEL_JOBS"
print_status "  Run Tests: $RUN_TESTS"
print_status "  Clean Build: $CLEAN_BUILD"

# Clean build directory if requested
if [[ "$CLEAN_BUILD" == true ]]; then
    print_status "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create the build directory if it doesn't exist
print_status "Creating build directory..."
mkdir -p "$BUILD_DIR" || {
    print_error "Failed to create build directory: $BUILD_DIR"
    exit 1
}

# Change to the build directory
cd "$BUILD_DIR" || {
    print_error "Failed to change to build directory: $BUILD_DIR"
    exit 1
}

# Set platform-specific CMake arguments
CMAKE_ARGS="-G \"$CMAKE_GENERATOR\" -DCMAKE_BUILD_TYPE=$BUILD_TYPE"

# Add platform-specific settings
if [[ "$OS" == "macos" ]]; then
    ARCH=$(uname -m)
    if [[ "$ARCH" == "arm64" ]]; then
        CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_OSX_ARCHITECTURES=arm64"
        print_status "Configuring for Apple Silicon (ARM64)"
    fi
fi

# Configure the project with CMake
print_status "Configuring CMake..."
if [[ "$VERBOSE" == true ]]; then
    eval "cmake $CMAKE_ARGS \"$PROJECT_ROOT\""
else
    eval "cmake $CMAKE_ARGS \"$PROJECT_ROOT\"" > cmake_config.log 2>&1
fi

if [[ $? -ne 0 ]]; then
    print_error "CMake configuration failed"
    if [[ "$VERBOSE" == false && -f cmake_config.log ]]; then
        print_status "CMake configuration log:"
        cat cmake_config.log
    fi
    exit 1
fi

print_success "CMake configuration completed"

# Build the project
print_status "Building project with $PARALLEL_JOBS parallel jobs..."
BUILD_ARGS="--build . --config $BUILD_TYPE --parallel $PARALLEL_JOBS"

if [[ "$VERBOSE" == true ]]; then
    BUILD_ARGS="$BUILD_ARGS --verbose"
fi

if cmake $BUILD_ARGS; then
    print_success "Build completed successfully"
else
    print_error "Build failed"
    exit 1
fi

# Show build artifacts
print_status "Build artifacts:"
if [[ -d "bin" ]]; then
    ls -la bin/
else
    print_warning "No bin directory found"
fi

# Run tests if requested
if [[ "$RUN_TESTS" == true ]]; then
    print_status "Running tests..."
    if command -v ctest &> /dev/null; then
        if ctest --output-on-failure --parallel "$PARALLEL_JOBS"; then
            print_success "All tests passed"
        else
            print_warning "Some tests failed"
            exit 1
        fi
    else
        print_warning "ctest not found - skipping tests"
    fi
fi

print_success "Build script completed successfully"

# Show next steps
echo ""
print_status "Next steps:"
if [[ -f "bin/fix-fasttrade" ]]; then
    echo "  ./bin/fix-fasttrade"
elif [[ -f "bin/$BUILD_TYPE/fix-fasttrade" ]]; then
    echo "  ./bin/$BUILD_TYPE/fix-fasttrade"
else
    echo "  Check the bin/ directory for executables"
fi

if [[ "$RUN_TESTS" == false ]]; then
    echo "  To run tests: ctest --output-on-failure"
fi

echo "  To install: cmake --build . --target install"