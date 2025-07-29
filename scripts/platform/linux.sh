#!/bin/bash

# Linux-specific setup script for FIX-FastTrade
# This script handles Linux-specific dependency installation

set -e

# Source the main setup functions if available
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [[ -f "$SCRIPT_DIR/../setup.sh" ]]; then
    source "$SCRIPT_DIR/../setup.sh"
fi

# Detect Linux distribution
detect_linux_distro() {
    if [[ -f /etc/os-release ]]; then
        source /etc/os-release
        echo "$ID"
    elif [[ -f /etc/redhat-release ]]; then
        echo "rhel"
    elif [[ -f /etc/debian_version ]]; then
        echo "debian"
    else
        echo "unknown"
    fi
}

# Install dependencies using apt (Debian/Ubuntu)
install_apt_dependencies() {
    print_status "Installing dependencies using apt..."
    
    # Update package lists
    print_status "Updating package lists..."
    sudo apt-get update || {
        print_error "Failed to update package lists"
        return 1
    }
    
    # Core build dependencies
    print_status "Installing core build dependencies..."
    local core_packages=(
        "build-essential"
        "cmake"
        "pkg-config"
        "git"
        "curl"
        "wget"
    )
    
    sudo apt-get install -y "${core_packages[@]}" || {
        print_error "Failed to install core build dependencies"
        return 1
    }
    
    # Boost libraries
    print_status "Installing Boost libraries..."
    sudo apt-get install -y libboost-all-dev || {
        print_error "Failed to install Boost libraries"
        return 1
    }
    
    # QuickFIX (may not be available on all distributions)
    print_status "Installing QuickFIX..."
    if sudo apt-get install -y libquickfix-dev 2>/dev/null; then
        print_success "QuickFIX installed via apt"
    else
        print_warning "QuickFIX not available via apt - will use submodule"
    fi
    
    # Optional performance libraries
    print_status "Installing optional performance libraries..."
    local optional_packages=(
        "libtbb-dev"
        "libbenchmark-dev"
        "doxygen"
        "graphviz"
    )
    
    for package in "${optional_packages[@]}"; do
        if sudo apt-get install -y "$package" 2>/dev/null; then
            print_success "$package installed"
        else
            print_warning "$package not available or failed to install"
        fi
    done
    
    print_success "apt dependencies installation complete"
    return 0
}

# Install dependencies using yum (RHEL/CentOS/Fedora old)
install_yum_dependencies() {
    print_status "Installing dependencies using yum..."
    
    # Install EPEL repository for additional packages
    if command -v yum-config-manager &> /dev/null; then
        sudo yum install -y epel-release || print_warning "EPEL repository not available"
    fi
    
    # Development tools
    print_status "Installing development tools..."
    sudo yum groupinstall -y "Development Tools" || {
        print_error "Failed to install development tools"
        return 1
    }
    
    # Core dependencies
    print_status "Installing core dependencies..."
    local core_packages=(
        "cmake"
        "pkgconfig"
        "git"
        "curl"
        "wget"
    )
    
    sudo yum install -y "${core_packages[@]}" || {
        print_error "Failed to install core dependencies"
        return 1
    }
    
    # Boost libraries
    print_status "Installing Boost libraries..."
    sudo yum install -y boost-devel || {
        print_error "Failed to install Boost libraries"
        return 1
    }
    
    # Optional libraries
    print_status "Installing optional libraries..."
    local optional_packages=(
        "tbb-devel"
        "doxygen"
        "graphviz"
    )
    
    for package in "${optional_packages[@]}"; do
        if sudo yum install -y "$package" 2>/dev/null; then
            print_success "$package installed"
        else
            print_warning "$package not available or failed to install"
        fi
    done
    
    print_success "yum dependencies installation complete"
    return 0
}

# Install dependencies using dnf (Fedora)
install_dnf_dependencies() {
    print_status "Installing dependencies using dnf..."
    
    # Development tools
    print_status "Installing development tools..."
    sudo dnf groupinstall -y "Development Tools" "C Development Tools and Libraries" || {
        print_error "Failed to install development tools"
        return 1
    }
    
    # Core dependencies
    print_status "Installing core dependencies..."
    local core_packages=(
        "cmake"
        "pkgconfig"
        "git"
        "curl"
        "wget"
    )
    
    sudo dnf install -y "${core_packages[@]}" || {
        print_error "Failed to install core dependencies"
        return 1
    }
    
    # Boost libraries
    print_status "Installing Boost libraries..."
    sudo dnf install -y boost-devel || {
        print_error "Failed to install Boost libraries"
        return 1
    }
    
    # Optional libraries
    print_status "Installing optional libraries..."
    local optional_packages=(
        "tbb-devel"
        "benchmark-devel"
        "doxygen"
        "graphviz"
    )
    
    for package in "${optional_packages[@]}"; do
        if sudo dnf install -y "$package" 2>/dev/null; then
            print_success "$package installed"
        else
            print_warning "$package not available or failed to install"
        fi
    done
    
    print_success "dnf dependencies installation complete"
    return 0
}

# Install dependencies using pacman (Arch Linux)
install_pacman_dependencies() {
    print_status "Installing dependencies using pacman..."
    
    # Update package database
    sudo pacman -Sy || {
        print_error "Failed to update package database"
        return 1
    }
    
    # Core dependencies
    print_status "Installing core dependencies..."
    local core_packages=(
        "base-devel"
        "cmake"
        "pkgconfig"
        "git"
        "curl"
        "wget"
    )
    
    sudo pacman -S --noconfirm "${core_packages[@]}" || {
        print_error "Failed to install core dependencies"
        return 1
    }
    
    # Boost libraries
    print_status "Installing Boost libraries..."
    sudo pacman -S --noconfirm boost || {
        print_error "Failed to install Boost libraries"
        return 1
    }
    
    # Optional libraries
    print_status "Installing optional libraries..."
    local optional_packages=(
        "intel-tbb"
        "benchmark"
        "doxygen"
        "graphviz"
    )
    
    for package in "${optional_packages[@]}"; do
        if sudo pacman -S --noconfirm "$package" 2>/dev/null; then
            print_success "$package installed"
        else
            print_warning "$package not available or failed to install"
        fi
    done
    
    print_success "pacman dependencies installation complete"
    return 0
}

# Linux-specific dependency installation
install_linux_dependencies() {
    print_status "Installing Linux dependencies..."
    
    local distro=$(detect_linux_distro)
    print_status "Detected Linux distribution: $distro"
    
    case $distro in
        "ubuntu"|"debian"|"linuxmint"|"pop")
            install_apt_dependencies
            ;;
        "fedora")
            install_dnf_dependencies
            ;;
        "rhel"|"centos"|"rocky"|"almalinux")
            if command -v dnf &> /dev/null; then
                install_dnf_dependencies
            else
                install_yum_dependencies
            fi
            ;;
        "arch"|"manjaro")
            install_pacman_dependencies
            ;;
        *)
            print_error "Unsupported Linux distribution: $distro"
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
            ;;
    esac
}

# Linux-specific environment setup
setup_linux_environment() {
    print_status "Setting up Linux-specific environment..."
    
    # Set up library paths
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
    
    if [[ -d "/usr/local/lib64/pkgconfig" ]]; then
        export PKG_CONFIG_PATH="/usr/local/lib64/pkgconfig:$PKG_CONFIG_PATH"
    fi
    
    # Set compiler flags for performance
    export CXXFLAGS="-march=native -mtune=native $CXXFLAGS"
    
    print_success "Linux environment configured"
}

# Verify Linux-specific installations
verify_linux_setup() {
    print_status "Verifying Linux setup..."
    
    local verification_failed=0
    
    # Check build tools
    if command -v gcc &> /dev/null || command -v clang &> /dev/null; then
        if command -v gcc &> /dev/null; then
            print_success "GCC: OK ($(gcc --version | head -n1))"
        fi
        if command -v clang &> /dev/null; then
            print_success "Clang: OK ($(clang --version | head -n1))"
        fi
    else
        print_error "C++ Compiler: MISSING"
        ((verification_failed++))
    fi
    
    # Check CMake
    if command -v cmake &> /dev/null; then
        print_success "CMake: OK ($(cmake --version | head -n1 | cut -d' ' -f3))"
    else
        print_error "CMake: MISSING"
        ((verification_failed++))
    fi
    
    # Check pkg-config
    if command -v pkg-config &> /dev/null; then
        print_success "pkg-config: OK"
    else
        print_error "pkg-config: MISSING"
        ((verification_failed++))
    fi
    
    # Check Boost
    if pkg-config --exists boost || [[ -d "/usr/include/boost" ]] || [[ -d "/usr/local/include/boost" ]]; then
        print_success "Boost: OK"
    else
        print_warning "Boost: NOT FOUND"
        ((verification_failed++))
    fi
    
    # Check make
    if command -v make &> /dev/null; then
        print_success "Make: OK ($(make --version | head -n1))"
    else
        print_error "Make: MISSING"
        ((verification_failed++))
    fi
    
    if [[ $verification_failed -eq 0 ]]; then
        print_success "Linux setup verification passed"
        return 0
    else
        print_error "$verification_failed verification checks failed"
        return 1
    fi
}

# Main function for Linux setup
main_linux() {
    print_status "Starting Linux-specific setup..."
    
    setup_linux_environment
    install_linux_dependencies
    verify_linux_setup
    
    print_success "Linux setup complete!"
}

# If script is run directly (not sourced), execute main function
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main_linux "$@"
fi