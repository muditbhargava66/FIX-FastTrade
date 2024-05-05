#!/bin/bash

# Set the project root directory
PROJECT_ROOT=$(dirname "$(dirname "$(dirname "$(readlink -f "$0")")")")

# Set the documentation directory
DOCS_DIR="$PROJECT_ROOT/docs"

# Set the Doxygen configuration file
DOXYGEN_CONFIG="$PROJECT_ROOT/Doxyfile"

# Check if Doxygen is installed
if ! command -v doxygen &> /dev/null; then
    echo "Doxygen is not installed. Please install Doxygen and try again."
    exit 1
fi

# Create the documentation directory if it doesn't exist
mkdir -p "$DOCS_DIR"

# Generate the documentation using Doxygen
doxygen "$DOXYGEN_CONFIG"

# Check the documentation generation result
if [ $? -eq 0 ]; then
    echo "Documentation generated successfully!"
else
    echo "Failed to generate documentation. Please check the Doxygen output for details."
    exit 1
fi