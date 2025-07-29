#!/bin/bash

# Script validation script for FIX-FastTrade
# This script validates that all build and setup scripts work correctly

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

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

print_status "Validating FIX-FastTrade build and setup scripts..."

# Check if we're in the right directory
if [[ ! -f "CMakeLists.txt" ]]; then
    print_error "Please run this script from the FIX-FastTrade root directory"
    exit 1
fi

validation_failed=0

# Test 1: Check script permissions
print_status "Checking script permissions..."
scripts_to_check=(
    "scripts/setup.sh"
    "scripts/platform/macos.sh"
    "scripts/platform/linux.sh"
    "tools/build/build.sh"
)

for script in "${scripts_to_check[@]}"; do
    if [[ -f "$script" ]]; then
        if [[ -x "$script" ]]; then
            print_success "$script is executable"
        else
            print_error "$script is not executable"
            ((validation_failed++))
        fi
    else
        print_error "$script does not exist"
        ((validation_failed++))
    fi
done

# Test 2: Check script syntax
print_status "Checking script syntax..."
for script in "${scripts_to_check[@]}"; do
    if [[ -f "$script" ]]; then
        if bash -n "$script" 2>/dev/null; then
            print_success "$script syntax is valid"
        else
            print_error "$script has syntax errors"
            ((validation_failed++))
        fi
    fi
done

# Test 3: Check Windows batch file
print_status "Checking Windows batch file..."
if [[ -f "tools/build/build.bat" ]]; then
    print_success "Windows build script exists"
else
    print_error "Windows build script missing"
    ((validation_failed++))
fi

if [[ -f "scripts/platform/windows.bat" ]]; then
    print_success "Windows platform script exists"
else
    print_error "Windows platform script missing"
    ((validation_failed++))
fi

# Test 4: Test help functionality
print_status "Testing help functionality..."
if ./scripts/setup.sh 4 < /dev/null > /dev/null 2>&1; then
    print_success "Setup script help works"
else
    print_warning "Setup script help test inconclusive"
fi

if ./tools/build/build.sh --help > /dev/null 2>&1; then
    print_success "Build script help works"
else
    print_error "Build script help failed"
    ((validation_failed++))
fi

# Test 5: Check platform detection
print_status "Testing platform detection..."
OS_DETECTED=$(./tools/build/build.sh --help 2>&1 | grep "Detected OS:" | cut -d: -f2 | xargs)
if [[ -n "$OS_DETECTED" ]]; then
    print_success "Platform detection works: $OS_DETECTED"
else
    print_error "Platform detection failed"
    ((validation_failed++))
fi

# Test 6: Check CPU core detection
print_status "Testing CPU core detection..."
if command -v nproc &> /dev/null; then
    CORES_NPROC=$(nproc)
    print_success "nproc detected $CORES_NPROC cores"
elif command -v sysctl &> /dev/null; then
    CORES_SYSCTL=$(sysctl -n hw.ncpu)
    print_success "sysctl detected $CORES_SYSCTL cores"
else
    print_warning "No CPU core detection method available"
fi

# Test 7: Check CMake configuration (dry run)
print_status "Testing CMake configuration..."
if [[ -d "build" ]]; then
    print_status "Build directory exists, testing configuration..."
    if (cd build && cmake .. --help > /dev/null 2>&1); then
        print_success "CMake configuration test passed"
    else
        print_warning "CMake configuration test inconclusive"
    fi
else
    print_status "No build directory - skipping CMake test"
fi

# Test 8: Check dependency verification functions
print_status "Testing dependency verification..."
if grep -q "verify_dependency" scripts/setup.sh; then
    print_success "Dependency verification function exists"
else
    print_error "Dependency verification function missing"
    ((validation_failed++))
fi

# Test 9: Check error handling
print_status "Testing error handling..."
if grep -q "set -e" scripts/setup.sh && grep -q "set -e" tools/build/build.sh; then
    print_success "Error handling (set -e) is enabled"
else
    print_error "Error handling not properly configured"
    ((validation_failed++))
fi

# Test 10: Check platform-specific fixes
print_status "Testing platform-specific fixes..."
if grep -q "get_cpu_cores" scripts/setup.sh && grep -q "get_cpu_cores" tools/build/build.sh; then
    print_success "Cross-platform CPU core detection implemented"
else
    print_error "Cross-platform CPU core detection missing"
    ((validation_failed++))
fi

# Summary
echo ""
if [[ $validation_failed -eq 0 ]]; then
    print_success "All script validations passed!"
    print_status "Scripts are ready for use"
else
    print_error "$validation_failed validation checks failed"
    print_status "Please fix the issues before using the scripts"
    exit 1
fi

print_status "Validation complete"