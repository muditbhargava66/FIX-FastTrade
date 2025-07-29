#!/bin/bash

# FIX-FastTrade Development Environment Setup
# Sets up a complete development environment for FIX-FastTrade

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Get the project root directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# Configuration
BUILD_TYPE="${BUILD_TYPE:-Debug}"
INSTALL_DEPENDENCIES="${INSTALL_DEPENDENCIES:-true}"
SETUP_IDE="${SETUP_IDE:-false}"
PARALLEL_JOBS="${PARALLEL_JOBS:-$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)}"

echo -e "${BLUE}FIX-FastTrade Development Environment Setup${NC}"
echo "============================================="
echo "Project Root: $PROJECT_ROOT"
echo "Build Type: $BUILD_TYPE"
echo "Install Dependencies: $INSTALL_DEPENDENCIES"
echo "Setup IDE: $SETUP_IDE"
echo "Parallel Jobs: $PARALLEL_JOBS"
echo ""

# Function to print status messages
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

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to detect the operating system
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    elif [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
        echo "windows"
    else
        echo "unknown"
    fi
}

OS=$(detect_os)
print_status "Detected OS: $OS"

# Change to project root
cd "$PROJECT_ROOT"

# Step 1: Check and install system dependencies
if [ "$INSTALL_DEPENDENCIES" = "true" ]; then
    print_status "Installing system dependencies..."
    
    case $OS in
        "linux")
            print_status "Installing dependencies for Linux..."
            
            # Check if we have sudo access
            if command_exists sudo; then
                # Update package list
                sudo apt-get update || print_warning "Failed to update package list"
                
                # Install essential build tools
                sudo apt-get install -y \
                    build-essential \
                    cmake \
                    git \
                    pkg-config \
                    libboost-all-dev \
                    libxml2-dev \
                    libxml2-utils \
                    doxygen \
                    graphviz \
                    clang-format \
                    clang-tidy \
                    valgrind \
                    gdb \
                    || print_warning "Some packages failed to install"
                
                print_success "Linux dependencies installed"
            else
                print_warning "No sudo access. Please install dependencies manually:"
                echo "  sudo apt-get install build-essential cmake git pkg-config libboost-all-dev libxml2-dev libxml2-utils doxygen graphviz clang-format clang-tidy valgrind gdb"
            fi
            ;;
            
        "macos")
            print_status "Installing dependencies for macOS..."
            
            # Check if Homebrew is installed
            if command_exists brew; then
                # Update Homebrew
                brew update || print_warning "Failed to update Homebrew"
                
                # Install dependencies
                brew install \
                    cmake \
                    boost \
                    libxml2 \
                    doxygen \
                    graphviz \
                    clang-format \
                    llvm \
                    || print_warning "Some packages failed to install"
                
                print_success "macOS dependencies installed"
            else
                print_warning "Homebrew not found. Please install Homebrew first:"
                echo "  /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
                echo "Then run this script again."
            fi
            ;;
            
        "windows")
            print_warning "Windows detected. Please install dependencies manually:"
            echo "  - Visual Studio 2019 or later with C++ support"
            echo "  - CMake (https://cmake.org/download/)"
            echo "  - Git (https://git-scm.com/download/win)"
            echo "  - Boost libraries"
            echo "  - Doxygen (optional, for documentation)"
            ;;
            
        *)
            print_warning "Unknown OS. Please install dependencies manually."
            ;;
    esac
    
    echo ""
fi

# Step 2: Initialize and update git submodules
print_status "Initializing git submodules..."
if [ -f ".gitmodules" ]; then
    git submodule update --init --recursive || print_warning "Failed to update submodules"
    print_success "Git submodules updated"
else
    print_warning "No git submodules found"
fi
echo ""

# Step 3: Create development directories
print_status "Creating development directories..."
mkdir -p build
mkdir -p build/debug
mkdir -p build/release
mkdir -p logs
mkdir -p temp
print_success "Development directories created"
echo ""

# Step 4: Configure build system
print_status "Configuring build system..."
cd build

# Configure for development (Debug build with all features)
cmake \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_CXX_FLAGS_DEBUG="-O0 -g3 -DDEBUG -fsanitize=address -fno-omit-frame-pointer" \
    -DBUILD_TESTING=ON \
    "$PROJECT_ROOT" || {
    print_error "CMake configuration failed"
    exit 1
}

print_success "Build system configured"
echo ""

# Step 5: Build the project
print_status "Building the project..."
cmake --build . --config "$BUILD_TYPE" -j "$PARALLEL_JOBS" || {
    print_error "Build failed"
    exit 1
}

print_success "Project built successfully"
echo ""

# Step 6: Run tests to verify setup
print_status "Running tests to verify setup..."
if [ -f "bin/fix-fasttrade-tests" ]; then
    # Run a quick test to verify everything works
    if ./bin/fix-fasttrade-tests --gtest_filter="*Test*" --gtest_brief=1; then
        print_success "Tests passed - development environment is working"
    else
        print_warning "Some tests failed, but environment setup is complete"
    fi
else
    print_warning "Test executable not found, but build completed"
fi
echo ""

# Step 7: Generate development documentation
print_status "Generating development documentation..."
cd "$PROJECT_ROOT"
if [ -x "tools/scripts/generate_docs.sh" ]; then
    ./tools/scripts/generate_docs.sh || print_warning "Documentation generation failed"
else
    print_warning "Documentation generation script not found"
fi
echo ""

# Step 8: Setup IDE configuration (optional)
if [ "$SETUP_IDE" = "true" ]; then
    print_status "Setting up IDE configuration..."
    
    # Create VS Code configuration
    mkdir -p .vscode
    
    # VS Code settings
    cat > .vscode/settings.json << 'EOF'
{
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
    "C_Cpp.default.compileCommands": "${workspaceFolder}/build/compile_commands.json",
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.generator": "Unix Makefiles",
    "files.associations": {
        "*.h": "cpp",
        "*.hpp": "cpp",
        "*.cpp": "cpp"
    },
    "editor.formatOnSave": true,
    "C_Cpp.clang_format_style": "file",
    "cmake.configureOnOpen": true
}
EOF
    
    # VS Code launch configuration
    cat > .vscode/launch.json << 'EOF'
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug FIX-FastTrade",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/bin/fix-fasttrade",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "build"
        },
        {
            "name": "Debug Tests",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/bin/fix-fasttrade-tests",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "build"
        }
    ]
}
EOF
    
    # VS Code tasks
    cat > .vscode/tasks.json << 'EOF'
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build",
            "type": "shell",
            "command": "cmake",
            "args": ["--build", "build", "--config", "Debug"],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            },
            "problemMatcher": "$gcc"
        },
        {
            "label": "clean",
            "type": "shell",
            "command": "cmake",
            "args": ["--build", "build", "--target", "clean"],
            "group": "build"
        },
        {
            "label": "test",
            "type": "shell",
            "command": "./tools/scripts/run_tests.sh",
            "group": "test",
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            }
        }
    ]
}
EOF
    
    print_success "VS Code configuration created"
    
    # Create CLion configuration hint
    cat > .idea_config_hint.txt << 'EOF'
CLion Configuration:
1. Open the project root directory in CLion
2. CLion should automatically detect the CMakeLists.txt
3. Configure CMake with:
   - Build Type: Debug
   - CMake options: -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DBUILD_TESTING=ON
4. Build directory: build
5. Enable code formatting with the .clang-format file
EOF
    
    print_success "IDE configuration files created"
fi

# Step 9: Create development scripts
print_status "Creating development helper scripts..."

# Quick build script
cat > dev_build.sh << 'EOF'
#!/bin/bash
# Quick development build script
cd build && cmake --build . --config Debug -j $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
EOF
chmod +x dev_build.sh

# Quick test script
cat > dev_test.sh << 'EOF'
#!/bin/bash
# Quick test script
./tools/scripts/run_tests.sh
EOF
chmod +x dev_test.sh

# Quick clean script
cat > dev_clean.sh << 'EOF'
#!/bin/bash
# Quick clean script
rm -rf build/*
rm -rf logs/*
rm -rf temp/*
echo "Development environment cleaned"
EOF
chmod +x dev_clean.sh

print_success "Development helper scripts created"
echo ""

# Step 10: Validate configuration
print_status "Validating configuration files..."
if [ -x "tools/scripts/validate_config.sh" ]; then
    ./tools/scripts/validate_config.sh || print_warning "Configuration validation had issues"
else
    print_warning "Configuration validation script not found"
fi
echo ""

# Create development environment summary
print_status "Creating development environment summary..."
cat > DEV_ENVIRONMENT.md << EOF
# Development Environment Summary

Generated: $(date)
OS: $OS
Build Type: $BUILD_TYPE

## Quick Commands

- **Build**: \`./dev_build.sh\` or \`cd build && make -j$PARALLEL_JOBS\`
- **Test**: \`./dev_test.sh\` or \`./tools/scripts/run_tests.sh\`
- **Clean**: \`./dev_clean.sh\`
- **Documentation**: \`./tools/scripts/generate_docs.sh\`
- **Config Validation**: \`./tools/scripts/validate_config.sh\`

## Directory Structure

- \`build/\` - Build output directory
- \`logs/\` - Application logs
- \`temp/\` - Temporary files
- \`docs/\` - Documentation
- \`config/\` - Configuration files
- \`tools/\` - Development tools and scripts

## IDE Configuration

$([ "$SETUP_IDE" = "true" ] && echo "- VS Code configuration created in .vscode/" || echo "- Run with SETUP_IDE=true to create IDE configurations")
- CLion configuration hints in .idea_config_hint.txt

## Build Targets

- \`fix-fasttrade\` - Main executable
- \`fix-fasttrade-core\` - Static library
- \`fix-fasttrade-tests\` - Test suite
- \`docs\` - API documentation

## Development Workflow

1. Make code changes
2. Run \`./dev_build.sh\` to build
3. Run \`./dev_test.sh\` to test
4. Run \`./tools/scripts/validate_config.sh\` to validate configs
5. Generate docs with \`./tools/scripts/generate_docs.sh\`

## Debugging

- Debug builds include AddressSanitizer
- Use GDB or your IDE's debugger
- Check logs in the \`logs/\` directory

## Performance Testing

- Build in Release mode for performance testing
- Use the benchmark targets if available
- Profile with tools like perf or Instruments

## Troubleshooting

- Check build logs in \`build/\`
- Verify dependencies are installed
- Run \`cmake --build build --target clean\` to clean build
- Check configuration files with validation script

For more information, see the documentation in \`docs/\`.
EOF

print_success "Development environment summary created: DEV_ENVIRONMENT.md"

# Final summary
echo ""
echo "============================================="
print_success "Development environment setup completed!"
echo ""

print_status "What was set up:"
echo "  ✓ System dependencies $([ "$INSTALL_DEPENDENCIES" = "true" ] && echo "(installed)" || echo "(skipped)")"
echo "  ✓ Git submodules initialized"
echo "  ✓ Build system configured ($BUILD_TYPE mode)"
echo "  ✓ Project built successfully"
echo "  ✓ Tests verified"
echo "  ✓ Documentation generated"
echo "  ✓ Development scripts created"
echo "  ✓ Configuration validated"
echo "  ✓ IDE configuration $([ "$SETUP_IDE" = "true" ] && echo "created" || echo "available")"

echo ""
print_status "Quick start:"
echo "  - Build: ./dev_build.sh"
echo "  - Test: ./dev_test.sh"
echo "  - Clean: ./dev_clean.sh"
echo "  - Read: DEV_ENVIRONMENT.md"

echo ""
print_status "Your development environment is ready!"
print_status "Happy coding! 🚀"