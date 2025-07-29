#!/bin/bash

# Configuration Validation Script for FIX-FastTrade
# This script validates all XML configuration files against the schema

echo "=== FIX-FastTrade Configuration Validation ==="
echo

# Check if xmllint is available
if ! command -v xmllint &> /dev/null; then
    echo "ERROR: xmllint is not installed. Please install libxml2-utils."
    exit 1
fi

# Schema file
SCHEMA="config/schemas/fix-config.xsd"

if [ ! -f "$SCHEMA" ]; then
    echo "ERROR: Schema file not found: $SCHEMA"
    exit 1
fi

echo "Using schema: $SCHEMA"
echo

# List of configuration files to validate
CONFIG_FILES=(
    "config/fix-config.xml"
    "config/examples/basic-config.xml"
    "config/examples/advanced-config.xml"
    "config/examples/test-config.xml"
)

# Validation results
PASSED=0
FAILED=0

# Validate each configuration file
for config in "${CONFIG_FILES[@]}"; do
    echo -n "Validating $config... "
    
    if [ ! -f "$config" ]; then
        echo "FAILED (file not found)"
        ((FAILED++))
        continue
    fi
    
    # Validate against schema
    if xmllint --schema "$SCHEMA" --noout "$config" 2>/dev/null; then
        echo "PASSED"
        ((PASSED++))
    else
        echo "FAILED"
        ((FAILED++))
        # Show detailed error
        echo "  Error details:"
        xmllint --schema "$SCHEMA" --noout "$config" 2>&1 | sed 's/^/    /'
    fi
done

echo
echo "=== Validation Summary ==="
echo "Passed: $PASSED"
echo "Failed: $FAILED"
echo "Total:  $((PASSED + FAILED))"

if [ $FAILED -eq 0 ]; then
    echo
    echo "✅ All configuration files are valid!"
    exit 0
else
    echo
    echo "❌ Some configuration files failed validation."
    exit 1
fi