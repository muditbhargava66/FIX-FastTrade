#!/bin/bash

# FIX-FastTrade Setup Script
# This script sets up the development environment and dependencies

set -e  # Exit on any error

echo "🚀 Setting up FIX-FastTrade development environment..."

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

# Detect OS and architecture
OS="unknown"
ARCH=$(uname -m)
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    OS="windows"
fi

print_status "Detected OS: $OS ($ARCH)"

# Check if we're in the right directory
if [[ ! -f "CMakeLists.txt" ]]; then
    print_error "Please run this script from the FIX-FastTrade root directory"
    exit 1
fi

# Verify dependencies function
verify_dependency() {
    local dep_name="$1"
    local check_command="$2"
    local install_hint="$3"
    
    if eval "$check_command" &> /dev/null; then
        print_success "$dep_name is available"
        return 0
    else
        print_warning "$dep_name not found. $install_hint"
        return 1
    fi
}

# Check system requirements
check_system_requirements() {
    print_status "Checking system requirements..."
    
    local missing_deps=0
    
    # Check CMake
    if ! verify_dependency "CMake" "cmake --version" "Install CMake >= 3.14"; then
        ((missing_deps++))
    fi
    
    # Check C++ compiler
    if ! verify_dependency "C++ Compiler" "g++ --version || clang++ --version" "Install GCC or Clang"; then
        ((missing_deps++))
    fi
    
    # Check Git
    if ! verify_dependency "Git" "git --version" "Install Git"; then
        ((missing_deps++))
    fi
    
    # Check pkg-config
    if ! verify_dependency "pkg-config" "pkg-config --version" "Install pkg-config"; then
        ((missing_deps++))
    fi
    
    if [[ $missing_deps -gt 0 ]]; then
        print_error "$missing_deps required dependencies are missing"
        print_status "Please install the missing dependencies and run this script again"
        return 1
    fi
    
    print_success "All system requirements met"
    return 0
}

# Initialize git submodules if they exist
if [[ -f ".gitmodules" ]]; then
    print_status "Initializing git submodules..."
    git submodule update --init --recursive
    print_success "Git submodules initialized"
fi

# Install system dependencies based on OS
install_system_deps() {
    print_status "Installing system dependencies for $OS..."
    
    # Use platform-specific scripts if available
    local platform_script="scripts/platform/${OS}.sh"
    if [[ -f "$platform_script" ]]; then
        print_status "Using platform-specific script: $platform_script"
        # Call the platform-specific installation function directly
        if [[ "$OS" == "macos" ]]; then
            source "$platform_script"
            install_macos_dependencies
            return $?
        elif [[ "$OS" == "linux" ]]; then
            source "$platform_script"
            install_linux_dependencies
            return $?
        fi
    fi
    
    # Fallback to inline installation
    case $OS in
        "macos")
            print_status "Installing macOS dependencies..."
            if command -v brew &> /dev/null; then
                print_status "Updating Homebrew..."
                brew update || print_warning "Failed to update Homebrew"
                
                print_status "Installing core dependencies..."
                brew install cmake boost pkg-config || {
                    print_error "Failed to install core dependencies"
                    return 1
                }
                
                print_status "Installing QuickFIX..."
                brew install quickfix || print_warning "QuickFIX installation failed - will use submodule"
                
                # Optional dependencies
                print_status "Installing optional dependencies..."
                brew install intel-tbb google-benchmark doxygen || print_warning "Some optional dependencies not installed"
                
                print_success "macOS dependencies installed"
            else
                print_error "Homebrew not found. Please install Homebrew first:"
                echo "  /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
                return 1
            fi
            ;;
        "linux")
            print_status "Installing Linux dependencies..."
            if command -v apt-get &> /dev/null; then
                print_status "Updating package lists..."
                sudo apt-get update || {
                    print_error "Failed to update package lists"
                    return 1
                }
                
                print_status "Installing build tools..."
                sudo apt-get install -y build-essential cmake pkg-config git || {
                    print_error "Failed to install build tools"
                    return 1
                }
                
                print_status "Installing Boost libraries..."
                sudo apt-get install -y libboost-all-dev || {
                    print_error "Failed to install Boost"
                    return 1
                }
                
                print_status "Installing QuickFIX..."
                sudo apt-get install -y libquickfix-dev || print_warning "QuickFIX installation failed - will use submodule"
                
                print_status "Installing optional dependencies..."
                sudo apt-get install -y libtbb-dev libbenchmark-dev doxygen || print_warning "Some optional dependencies not installed"
                
                print_success "Linux (apt) dependencies installed"
                
            elif command -v yum &> /dev/null; then
                print_status "Installing dependencies with yum..."
                sudo yum groupinstall -y "Development Tools" || {
                    print_error "Failed to install development tools"
                    return 1
                }
                
                sudo yum install -y cmake pkgconfig boost-devel git || {
                    print_error "Failed to install core dependencies"
                    return 1
                }
                
                sudo yum install -y quickfix-devel tbb-devel doxygen || print_warning "Some dependencies not available via yum"
                
                print_success "Linux (yum) dependencies installed"
                
            elif command -v dnf &> /dev/null; then
                print_status "Installing dependencies with dnf..."
                sudo dnf groupinstall -y "Development Tools" || {
                    print_error "Failed to install development tools"
                    return 1
                }
                
                sudo dnf install -y cmake pkgconfig boost-devel git || {
                    print_error "Failed to install core dependencies"
                    return 1
                }
                
                sudo dnf install -y quickfix-devel tbb-devel doxygen || print_warning "Some dependencies not available via dnf"
                
                print_success "Linux (dnf) dependencies installed"
            else
                print_error "No supported package manager found (apt-get, yum, dnf)"
                print_status "Please install dependencies manually:"
                echo "  - build-essential or equivalent"
                echo "  - cmake (>= 3.14)"
                echo "  - pkg-config"
                echo "  - libboost-all-dev (>= 1.71)"
                echo "  - libquickfix-dev (optional)"
                echo "  - libtbb-dev (optional)"
                echo "  - libbenchmark-dev (optional)"
                echo "  - doxygen (optional)"
                return 1
            fi
            ;;
        *)
            print_error "Unsupported OS: $OS"
            print_status "Please install dependencies manually or use platform-specific scripts"
            return 1
            ;;
    esac
}

# Build third-party dependencies if using submodules
build_third_party() {
    print_status "Building third-party dependencies..."
    local cpu_cores=$(get_cpu_cores)
    local build_failed=0
    
    # Initialize submodules if they exist
    if [[ -f ".gitmodules" ]]; then
        print_status "Initializing git submodules..."
        git submodule update --init --recursive || {
            print_error "Failed to initialize git submodules"
            return 1
        }
        print_success "Git submodules initialized"
    fi
    
    # Build QuickFIX if present and not already built
    if [[ -d "third_party/quickfix" ]]; then
        if [[ -f "third_party/quickfix/lib/libquickfix.a" || -f "third_party/quickfix/build/lib/libquickfix.a" ]]; then
            print_success "QuickFIX already built"
        else
            print_status "Building QuickFIX from source..."
            (
                cd third_party/quickfix || exit 1
                mkdir -p build && cd build || exit 1
                
                # Configure with appropriate settings
                cmake .. \
                    -DCMAKE_BUILD_TYPE=Release \
                    -DHAVE_SSL=OFF \
                    -DHAVE_MYSQL=OFF \
                    -DHAVE_POSTGRESQL=OFF \
                    || exit 1
                
                # Build with all available cores
                make -j"$cpu_cores" || exit 1
            ) || {
                print_error "Failed to build QuickFIX"
                ((build_failed++))
            }
            
            if [[ $build_failed -eq 0 ]]; then
                print_success "QuickFIX built successfully"
            fi
        fi
    else
        print_status "QuickFIX submodule not found - will use system installation"
    fi
    
    # Build GoogleTest if present and not already built
    if [[ -d "third_party/googletest" ]]; then
        if [[ -f "third_party/googletest/build/lib/libgtest.a" ]]; then
            print_success "GoogleTest already built"
        else
            print_status "Building GoogleTest from source..."
            (
                cd third_party/googletest || exit 1
                mkdir -p build && cd build || exit 1
                
                cmake .. -DCMAKE_BUILD_TYPE=Release || exit 1
                make -j"$cpu_cores" || exit 1
            ) || {
                print_error "Failed to build GoogleTest"
                ((build_failed++))
            }
            
            if [[ $build_failed -eq 0 ]]; then
                print_success "GoogleTest built successfully"
            fi
        fi
    else
        print_status "GoogleTest submodule not found - will use system installation"
    fi
    
    if [[ $build_failed -gt 0 ]]; then
        print_warning "$build_failed third-party dependencies failed to build"
        print_status "You may need to install system packages instead"
        return 1
    fi
    
    print_success "All third-party dependencies processed"
    return 0
}

# Main setup process
main() {
    print_status "Starting FIX-FastTrade setup process..."
    
    # Check system requirements first
    if ! check_system_requirements; then
        print_error "System requirements not met. Please install missing dependencies."
        exit 1
    fi
    
    # Ask user what they want to do
    echo ""
    echo "Choose setup option:"
    echo "1) Install system dependencies only"
    echo "2) Build from git submodules (source)"
    echo "3) Both system deps + submodules (recommended)"
    echo "4) Skip dependency installation (configure only)"
    echo "5) Full setup with verification"
    echo ""
    read -p "Enter choice [1-5]: " choice
    
    local setup_failed=0
    
    case $choice in
        1)
            print_status "Installing system dependencies..."
            install_system_deps || ((setup_failed++))
            ;;
        2)
            print_status "Building from submodules..."
            build_third_party || ((setup_failed++))
            ;;
        3)
            print_status "Installing system dependencies and building submodules..."
            install_system_deps || ((setup_failed++))
            build_third_party || ((setup_failed++))
            ;;
        4)
            print_status "Skipping dependency installation"
            ;;
        5)
            print_status "Full setup with verification..."
            install_system_deps || ((setup_failed++))
            build_third_party || ((setup_failed++))
            ;;
        *)
            print_error "Invalid choice"
            exit 1
            ;;
    esac
    
    if [[ $setup_failed -gt 0 ]]; then
        print_warning "Some setup steps failed, but continuing with build configuration..."
    fi
    
    # Create build directory
    print_status "Creating build directory..."
    mkdir -p build || {
        print_error "Failed to create build directory"
        exit 1
    }
    
    # Configure CMake
    print_status "Configuring CMake..."
    local cmake_args="-DCMAKE_BUILD_TYPE=Debug"
    
    # Add platform-specific CMake arguments
    if [[ "$OS" == "macos" && "$ARCH" == "arm64" ]]; then
        cmake_args="$cmake_args -DCMAKE_OSX_ARCHITECTURES=arm64"
    fi
    
    (
        cd build || exit 1
        cmake .. $cmake_args || exit 1
    ) || {
        print_error "CMake configuration failed"
        exit 1
    }
    
    # Verify build configuration if requested
    if [[ $choice -eq 5 ]]; then
        print_status "Verifying build configuration..."
        (
            cd build || exit 1
            make -j1 --dry-run > /dev/null || exit 1
        ) || {
            print_error "Build verification failed"
            exit 1
        }
        print_success "Build configuration verified"
    fi
    
    local cpu_cores=$(get_cpu_cores)
    
    print_success "Setup complete!"
    echo ""
    echo "Next steps:"
    echo "  cd build"
    echo "  make -j$cpu_cores"
    echo "  ./bin/fix-fasttrade"
    echo ""
    echo "Available targets:"
    echo "  make fix-fasttrade      # Main executable"
    echo "  make fix-fasttrade-core # Static library"
    echo "  make test              # Run tests"
    echo "  make docs              # Generate documentation"
    echo ""
    echo "For development:"
    echo "  make -j$cpu_cores && make test"
    echo ""
}

# Run main function
main "$@"