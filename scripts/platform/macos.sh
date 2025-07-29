#!/bin/bash

# macOS-specific setup script for FIX-FastTrade
# This script handles macOS-specific dependency installation

set -e

# Source the main setup functions if available
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [[ -f "$SCRIPT_DIR/../setup.sh" ]]; then
    source "$SCRIPT_DIR/../setup.sh"
fi

# macOS-specific dependency installation
install_macos_dependencies() {
    print_status "Installing macOS dependencies..."
    
    # Check for Homebrew
    if ! command -v brew &> /dev/null; then
        print_error "Homebrew not found. Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)" || {
            print_error "Failed to install Homebrew"
            return 1
        }
        
        # Add Homebrew to PATH for current session
        if [[ -f "/opt/homebrew/bin/brew" ]]; then
            eval "$(/opt/homebrew/bin/brew shellenv)"
        elif [[ -f "/usr/local/bin/brew" ]]; then
            eval "$(/usr/local/bin/brew shellenv)"
        fi
    fi
    
    print_status "Updating Homebrew..."
    brew update || print_warning "Failed to update Homebrew"
    
    # Core build dependencies
    print_status "Installing core build dependencies..."
    local core_deps=(
        "cmake"
        "pkg-config" 
        "git"
    )
    
    for dep in "${core_deps[@]}"; do
        if brew list "$dep" &> /dev/null; then
            print_success "$dep already installed"
        else
            print_status "Installing $dep..."
            brew install "$dep" || {
                print_error "Failed to install $dep"
                return 1
            }
        fi
    done
    
    # Boost libraries
    print_status "Installing Boost libraries..."
    if brew list boost &> /dev/null; then
        print_success "Boost already installed"
    else
        brew install boost || {
            print_error "Failed to install Boost"
            return 1
        }
    fi
    
    # QuickFIX (optional - may not be available via Homebrew)
    print_status "Installing QuickFIX..."
    if brew list quickfix &> /dev/null; then
        print_success "QuickFIX already installed"
    else
        # Try to install QuickFIX, but don't fail if it's not available
        if brew install quickfix 2>/dev/null; then
            print_success "QuickFIX installed via Homebrew"
        else
            print_warning "QuickFIX not available via Homebrew - will use submodule"
        fi
    fi
    
    # Optional performance libraries
    print_status "Installing optional performance libraries..."
    local optional_deps=(
        "intel-tbb"
        "google-benchmark"
        "doxygen"
    )
    
    for dep in "${optional_deps[@]}"; do
        if brew list "$dep" &> /dev/null; then
            print_success "$dep already installed"
        else
            if brew install "$dep" 2>/dev/null; then
                print_success "$dep installed"
            else
                print_warning "$dep not available or failed to install"
            fi
        fi
    done
    
    # macOS-specific: Install Xcode command line tools if not present
    if ! xcode-select -p &> /dev/null; then
        print_status "Installing Xcode command line tools..."
        xcode-select --install || print_warning "Xcode command line tools installation may have failed"
    else
        print_success "Xcode command line tools already installed"
    fi
    
    print_success "macOS dependencies installation complete"
    return 0
}

# macOS-specific environment setup
setup_macos_environment() {
    print_status "Setting up macOS-specific environment..."
    
    # Set up environment variables for Apple Silicon
    if [[ "$(uname -m)" == "arm64" ]]; then
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
    
    print_success "macOS environment configured"
}

# Verify macOS-specific installations
verify_macos_setup() {
    print_status "Verifying macOS setup..."
    
    local verification_failed=0
    
    # Check Xcode tools
    if xcode-select -p &> /dev/null; then
        print_success "Xcode command line tools: OK"
    else
        print_error "Xcode command line tools: MISSING"
        ((verification_failed++))
    fi
    
    # Check Homebrew
    if command -v brew &> /dev/null; then
        print_success "Homebrew: OK ($(brew --version | head -n1))"
    else
        print_error "Homebrew: MISSING"
        ((verification_failed++))
    fi
    
    # Check CMake
    if command -v cmake &> /dev/null; then
        print_success "CMake: OK ($(cmake --version | head -n1 | cut -d' ' -f3))"
    else
        print_error "CMake: MISSING"
        ((verification_failed++))
    fi
    
    # Check Boost
    if brew list boost &> /dev/null || pkg-config --exists boost; then
        print_success "Boost: OK"
    else
        print_warning "Boost: NOT FOUND"
        ((verification_failed++))
    fi
    
    # Check compiler
    if command -v clang++ &> /dev/null; then
        print_success "C++ Compiler: OK ($(clang++ --version | head -n1))"
    else
        print_error "C++ Compiler: MISSING"
        ((verification_failed++))
    fi
    
    if [[ $verification_failed -eq 0 ]]; then
        print_success "macOS setup verification passed"
        return 0
    else
        print_error "$verification_failed verification checks failed"
        return 1
    fi
}

# Main function for macOS setup
main_macos() {
    print_status "Starting macOS-specific setup..."
    
    setup_macos_environment
    install_macos_dependencies
    verify_macos_setup
    
    print_success "macOS setup complete!"
}

# If script is run directly (not sourced), execute main function
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main_macos "$@"
fi