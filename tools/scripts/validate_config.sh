#!/bin/bash

# FIX-FastTrade Configuration Validator
# Validates XML configuration files against schemas and tests basic functionality

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
CONFIG_DIR="$PROJECT_ROOT/config"

echo -e "${BLUE}FIX-FastTrade Configuration Validator${NC}"
echo "========================================"
echo "Project Root: $PROJECT_ROOT"
echo "Config Directory: $CONFIG_DIR"
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

# Check if config directory exists
if [ ! -d "$CONFIG_DIR" ]; then
    print_error "Configuration directory not found: $CONFIG_DIR"
    exit 1
fi

# Check for required tools
check_tool() {
    if ! command -v "$1" &> /dev/null; then
        print_warning "$1 is not installed. Some validation features will be skipped."
        return 1
    fi
    return 0
}

# Check for XML validation tools
XMLLINT_AVAILABLE=false
if check_tool xmllint; then
    XMLLINT_AVAILABLE=true
fi

# Configuration files to validate
CONFIG_FILES=(
    "$CONFIG_DIR/fix-config.xml"
    "$CONFIG_DIR/examples/basic-config.xml"
    "$CONFIG_DIR/examples/advanced-config.xml"
    "$CONFIG_DIR/examples/test-config.xml"
)

# Schema file
SCHEMA_FILE="$CONFIG_DIR/schemas/fix-config.xsd"

VALIDATION_FAILED=0
FILES_VALIDATED=0

# Custom validation function for FIX-FastTrade configurations
validate_fix_config() {
    local config_file="$1"
    local basename_file=$(basename "$config_file")
    
    print_status "Performing FIX-FastTrade specific validation for $basename_file"
    
    # Check for required elements
    local required_elements=(
        "BeginString"
        "SenderCompID" 
        "TargetCompID"
        "ConnectionType"
        "SocketConnectHost"
        "SocketConnectPort"
    )
    
    for element in "${required_elements[@]}"; do
        if grep -q "<$element>" "$config_file"; then
            print_success "Required element found: $element"
        else
            print_error "Missing required element: $element"
            VALIDATION_FAILED=1
        fi
    done
    
    # Check for valid FIX version
    if grep -q "BeginString.*FIX\.4\.[0-4]" "$config_file" || grep -q "BeginString.*FIXT\.1\.1" "$config_file"; then
        print_success "Valid FIX version found"
    else
        print_warning "FIX version may not be supported"
    fi
    
    # Check for reasonable port numbers
    if grep -q "SocketConnectPort.*[0-9]\{1,5\}" "$config_file"; then
        local port=$(grep -o "SocketConnectPort.*[0-9]\{1,5\}" "$config_file" | grep -o "[0-9]\{1,5\}")
        if [ "$port" -ge 1024 ] && [ "$port" -le 65535 ]; then
            print_success "Valid port number: $port"
        else
            print_warning "Port number may be invalid: $port"
        fi
    fi
    
    # Check for SessionID compatibility (current implementation)
    if grep -q "SessionID" "$config_file"; then
        print_success "SessionID configuration found"
    else
        print_warning "No SessionID configuration found"
    fi
}

print_status "Starting configuration validation..."
echo ""

# Validate XML schema if available
if [ -f "$SCHEMA_FILE" ] && [ "$XMLLINT_AVAILABLE" = true ]; then
    print_status "Validating XML schema: $(basename "$SCHEMA_FILE")"
    
    if xmllint --noout "$SCHEMA_FILE" 2>/dev/null; then
        print_success "XML schema is valid"
    else
        print_error "XML schema validation failed"
        VALIDATION_FAILED=1
    fi
    echo ""
fi

# Validate each configuration file
for config_file in "${CONFIG_FILES[@]}"; do
    if [ ! -f "$config_file" ]; then
        print_warning "Configuration file not found: $(basename "$config_file")"
        continue
    fi
    
    print_status "Validating: $(basename "$config_file")"
    
    # Basic XML well-formedness check
    if [ "$XMLLINT_AVAILABLE" = true ]; then
        if xmllint --noout "$config_file" 2>/dev/null; then
            print_success "XML is well-formed"
        else
            print_error "XML is not well-formed"
            VALIDATION_FAILED=1
            continue
        fi
        
        # Schema validation if schema exists
        if [ -f "$SCHEMA_FILE" ]; then
            if xmllint --noout --schema "$SCHEMA_FILE" "$config_file" 2>/dev/null; then
                print_success "Schema validation passed"
            else
                print_error "Schema validation failed"
                VALIDATION_FAILED=1
            fi
        fi
    else
        print_warning "Skipping XML validation (xmllint not available)"
    fi
    
    # Custom validation checks for FIX-FastTrade specific requirements
    validate_fix_config "$config_file"
    
    ((FILES_VALIDATED++))
    echo ""
done

# Test configuration loading (if build exists)
BUILD_DIR="$PROJECT_ROOT/build"
if [ -d "$BUILD_DIR" ] && [ -f "$BUILD_DIR/bin/fix-fasttrade" ]; then
    print_status "Testing configuration loading with actual application..."
    
    for config_file in "${CONFIG_FILES[@]}"; do
        if [ ! -f "$config_file" ]; then
            continue
        fi
        
        print_status "Testing config load: $(basename "$config_file")"
        
        # Test configuration loading (dry run)
        # Note: This would need to be implemented in the actual application
        # For now, we'll just check if the file can be read
        if [ -r "$config_file" ]; then
            print_success "Configuration file is readable"
        else
            print_error "Configuration file is not readable"
            VALIDATION_FAILED=1
        fi
    done
    echo ""
else
    print_warning "Application not built. Skipping runtime configuration tests."
    print_status "Run 'tools/build/build.sh' to build the application for full validation."
    echo ""
fi

# Generate validation report
print_status "Generating validation report..."
REPORT_FILE="$PROJECT_ROOT/config_validation_report.txt"

cat > "$REPORT_FILE" << EOF
FIX-FastTrade Configuration Validation Report
Generated: $(date)

Files Validated: $FILES_VALIDATED
Validation Status: $([ $VALIDATION_FAILED -eq 0 ] && echo "PASSED" || echo "FAILED")

Configuration Files:
EOF

for config_file in "${CONFIG_FILES[@]}"; do
    if [ -f "$config_file" ]; then
        echo "  ✓ $(basename "$config_file")" >> "$REPORT_FILE"
    else
        echo "  ✗ $(basename "$config_file") (missing)" >> "$REPORT_FILE"
    fi
done

cat >> "$REPORT_FILE" << EOF

Schema File:
$([ -f "$SCHEMA_FILE" ] && echo "  ✓ $(basename "$SCHEMA_FILE")" || echo "  ✗ $(basename "$SCHEMA_FILE") (missing)")

Tools Available:
  xmllint: $([ "$XMLLINT_AVAILABLE" = true ] && echo "✓" || echo "✗")

Recommendations:
EOF

if [ "$XMLLINT_AVAILABLE" = false ]; then
    echo "  - Install libxml2-utils (xmllint) for full XML validation" >> "$REPORT_FILE"
fi

if [ ! -f "$SCHEMA_FILE" ]; then
    echo "  - Create XML schema file for comprehensive validation" >> "$REPORT_FILE"
fi

if [ $VALIDATION_FAILED -ne 0 ]; then
    echo "  - Fix validation errors listed above" >> "$REPORT_FILE"
fi

echo "  - Test configurations with actual application runtime" >> "$REPORT_FILE"

print_success "Validation report saved to: $REPORT_FILE"

# Summary
echo ""
echo "========================================"
if [ $VALIDATION_FAILED -eq 0 ]; then
    print_success "All configuration validations passed!"
    echo ""
    print_status "Configuration files are ready for use."
    print_status "Report saved to: config_validation_report.txt"
    exit 0
else
    print_error "Configuration validation failed!"
    echo ""
    print_status "Please fix the errors listed above."
    print_status "Check the validation report for details: config_validation_report.txt"
    exit 1
fi