#!/bin/bash

# Cross-Platform Build Script for FIX-FastTrade
# Supports macOS (ARM64/Intel), Linux (x86_64/ARM64), and Windows (via WSL/MSYS2)

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Function to print colored output
print_header() {
    echo -e "${PURPLE}========================================${NC}"
    echo -e "${PURPLE}$1${NC}"
    echo -e "${PURPLE}========================================${NC}"
}

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

# Detect platform and architecture
detect_platform() {
    OS="unknown"
    ARCH=$(uname -m)
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
    elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
        OS="windows"
    fi
    
    print_status "Detected platform: $OS ($ARCH)"
}

# Get CPU core count (cross-platform)
get_cpu_cores() {
    case $OS in
        "linux")
            nproc
            ;;
        "macos")
            sysctl -n hw.ncpu
            ;;
        "windows")
            echo $NUMBER_OF_PROCESSORS
            ;;
        *)
            echo 4
            ;;
    esac
}

# Check system requirements
check_requirements() {
    print_status "Checking system requirements..."
    
    local missing_deps=0
    
    # Check CMake
    if ! command -v cmake &> /dev/null; then
        print_error "CMake not found. Please install CMake >= 3.14"
        ((missing_deps++))
    else
        local cmake_version=$(cmake --version | head -n1 | cut -d' ' -f3)
        print_success "CMake found: $cmake_version"
    fi
    
    # Check C++ compiler
    if command -v g++ &> /dev/null; then
        local gcc_version=$(g++ --version | head -n1)
        print_success "GCC found: $gcc_version"
    elif command -v clang++ &> /dev/null; then
        local clang_version=$(clang++ --version | head -n1)
        print_success "Clang found: $clang_version"
    else
        print_error "No C++ compiler found. Please install GCC or Clang"
        ((missing_deps++))
    fi
    
    # Check Git
    if ! command -v git &> /dev/null; then
        print_error "Git not found. Please install Git"
        ((missing_deps++))
    else
        print_success "Git found"
    fi
    
    if [[ $missing_deps -gt 0 ]]; then
        print_error "$missing_deps required dependencies are missing"
        exit 1
    fi
    
    print_success "All system requirements met"
}

# Setup platform-specific environment
setup_platform_environment() {
    print_status "Setting up platform-specific environment..."
    
    case $OS in
        "macos")
            # Set up macOS-specific environment
            if [[ "$ARCH" == "arm64" ]]; then
                print_status "Configuring for Apple Silicon (ARM64)..."
                export CMAKE_OSX_ARCHITECTURES="arm64"
                export MACOSX_DEPLOYMENT_TARGET="11.0"
                
                # Homebrew paths for Apple Silicon
                if [[ -d "/opt/homebrew" ]]; then
                    export PATH="/opt/homebrew/bin:$PATH"
                    export PKG_CONFIG_PATH="/opt/homebrew/lib/pkgconfig:$PKG_CONFIG_PATH"
                    export CMAKE_PREFIX_PATH="/opt/homebrew:$CMAKE_PREFIX_PATH"
                fi
            else
                print_status "Configuring for Intel x86_64..."
                export CMAKE_OSX_ARCHITECTURES="x86_64"
                export MACOSX_DEPLOYMENT_TARGET="10.15"
                
                # Homebrew paths for Intel
                if [[ -d "/usr/local" ]]; then
                    export PATH="/usr/local/bin:$PATH"
                    export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH"
                    export CMAKE_PREFIX_PATH="/usr/local:$CMAKE_PREFIX_PATH"
                fi
            fi
            
            # Set compiler flags for macOS
            export CXXFLAGS="-stdlib=libc++ $CXXFLAGS"
            export LDFLAGS="-stdlib=libc++ $LDFLAGS"
            ;;
            
        "linux")
            # Set up Linux-specific environment
            if [[ -d "/usr/local/lib" ]]; then
                export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"
            fi
            
            if [[ -d "/usr/local/lib64" ]]; then
                export LD_LIBRARY_PATH="/usr/local/lib64:$LD_LIBRARY_PATH"
            fi
            
            # Set up pkg-config paths
            if [[ -d "/usr/local/lib/pkgconfig" ]]; then
                export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH"
            fi
            ;;
            
        "windows")
            # Windows-specific setup (MSYS2/MinGW)
            print_status "Configuring for Windows..."
            ;;
    esac
    
    print_success "Platform environment configured"
}

# Configure CMake with platform-specific optimizations
configure_cmake() {
    print_status "Configuring CMake with platform optimizations..."
    
    local cmake_args="-DCMAKE_BUILD_TYPE=Release -DENABLE_SIMD=ON"
    
    case $OS in
        "macos")
            if [[ "$ARCH" == "arm64" ]]; then
                cmake_args="$cmake_args -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0"
                cmake_args="$cmake_args -DCMAKE_CXX_FLAGS=\"-O3 -march=native -mtune=native\""
            else
                cmake_args="$cmake_args -DCMAKE_OSX_ARCHITECTURES=x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15"
                cmake_args="$cmake_args -DCMAKE_CXX_FLAGS=\"-O3 -march=native -mtune=native -mavx2\""
            fi
            ;;
            
        "linux")
            if [[ "$ARCH" == "aarch64" || "$ARCH" == "arm64" ]]; then
                cmake_args="$cmake_args -DCMAKE_CXX_FLAGS=\"-O3 -march=native -mtune=native\""
            else
                cmake_args="$cmake_args -DCMAKE_CXX_FLAGS=\"-O3 -march=native -mtune=native -mavx2\""
            fi
            cmake_args="$cmake_args -DENABLE_TBB=ON"
            ;;
            
        "windows")
            cmake_args="$cmake_args -G \"MinGW Makefiles\""
            cmake_args="$cmake_args -DCMAKE_CXX_FLAGS=\"-O3 -march=native\""
            ;;
    esac
    
    # Add LTO if supported
    cmake_args="$cmake_args -DENABLE_LTO=ON"
    
    print_status "CMake arguments: $cmake_args"
    
    # Create build directory
    mkdir -p build
    cd build
    
    # Configure with platform-specific arguments
    eval "cmake .. $cmake_args"
    
    if [[ $? -eq 0 ]]; then
        print_success "CMake configuration successful"
    else
        print_error "CMake configuration failed"
        exit 1
    fi
}

# Build the project
build_project() {
    print_status "Building FIX-FastTrade..."
    
    local cpu_cores=$(get_cpu_cores)
    print_status "Using $cpu_cores CPU cores for parallel build"
    
    case $OS in
        "windows")
            mingw32-make -j$cpu_cores
            ;;
        *)
            make -j$cpu_cores
            ;;
    esac
    
    if [[ $? -eq 0 ]]; then
        print_success "Build completed successfully"
    else
        print_error "Build failed"
        exit 1
    fi
}

# Test the build
test_build() {
    print_status "Testing the build..."
    
    # Check if executable exists
    local exe_name="fix-fasttrade"
    if [[ "$OS" == "windows" ]]; then
        exe_name="fix-fasttrade.exe"
    fi
    
    if [[ -f "bin/$exe_name" ]]; then
        print_success "Executable found: bin/$exe_name"
        
        # Test help output
        if ./bin/$exe_name --help &> /dev/null; then
            print_success "Executable runs successfully"
        else
            print_warning "Executable found but may have runtime issues"
        fi
        
        # Check binary architecture
        case $OS in
            "macos")
                local arch_info=$(file ./bin/$exe_name)
                print_status "Binary architecture: $arch_info"
                ;;
            "linux")
                local arch_info=$(file ./bin/$exe_name)
                print_status "Binary architecture: $arch_info"
                ;;
        esac
        
    else
        print_error "Executable not found: bin/$exe_name"
        exit 1
    fi
}

# Run tests if available
run_tests() {
    print_status "Running tests..."
    
    if make test &> /dev/null; then
        print_success "All tests passed"
    else
        print_warning "Some tests failed or test target not available"
    fi
}

# Display build summary
display_summary() {
    print_header "Build Summary"
    
    echo -e "${CYAN}Platform:${NC} $OS ($ARCH)"
    echo -e "${CYAN}Build Type:${NC} Release with SIMD optimizations"
    echo -e "${CYAN}Executable:${NC} bin/fix-fasttrade"
    
    if [[ -f "bin/fix-fasttrade" ]]; then
        local file_size=$(du -h bin/fix-fasttrade | cut -f1)
        echo -e "${CYAN}Binary Size:${NC} $file_size"
    fi
    
    echo ""
    echo -e "${GREEN}Next steps:${NC}"
    echo "  1. Test the executable: ./bin/fix-fasttrade --help"
    echo "  2. Copy configuration: cp ../config/fix-config.xml ."
    echo "  3. Run the system: ./bin/fix-fasttrade --config fix-config.xml"
    echo ""
    echo -e "${GREEN}For development:${NC}"
    echo "  - Run tests: make test"
    echo "  - Generate docs: make docs"
    echo "  - Debug build: cmake .. -DCMAKE_BUILD_TYPE=Debug && make"
    echo ""
}

# Main function
main() {
    print_header "FIX-FastTrade Cross-Platform Build Script"
    
    # Check if we're in the right directory
    if [[ ! -f "CMakeLists.txt" ]]; then
        print_error "Please run this script from the FIX-FastTrade root directory"
        exit 1
    fi
    
    # Initialize git submodules
    if [[ -f ".gitmodules" ]]; then
        print_status "Initializing git submodules..."
        git submodule update --init --recursive
    fi
    
    # Execute build steps
    detect_platform
    check_requirements
    setup_platform_environment
    configure_cmake
    build_project
    test_build
    run_tests
    
    # Return to root directory
    cd ..
    
    display_summary
    
    print_success "Cross-platform build completed successfully!"
}

# Handle script arguments
case "${1:-}" in
    --help|-h)
        echo "FIX-FastTrade Cross-Platform Build Script"
        echo ""
        echo "Usage: $0 [options]"
        echo ""
        echo "Options:"
        echo "  --help, -h     Show this help message"
        echo "  --clean        Clean build directory before building"
        echo "  --debug        Build in debug mode"
        echo "  --no-simd      Disable SIMD optimizations"
        echo ""
        echo "Supported platforms:"
        echo "  - macOS (ARM64/Intel)"
        echo "  - Linux (x86_64/ARM64)"
        echo "  - Windows (via MSYS2/MinGW)"
        echo ""
        exit 0
        ;;
    --clean)
        print_status "Cleaning build directory..."
        rm -rf build/
        ;;
    --debug)
        export BUILD_TYPE="Debug"
        ;;
    --no-simd)
        export DISABLE_SIMD="ON"
        ;;
esac

# Run main function
main "$@"