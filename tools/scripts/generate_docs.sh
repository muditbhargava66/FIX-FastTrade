#!/bin/bash

# FIX-FastTrade Documentation Generator
# Generates API documentation using Doxygen and creates additional documentation

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
BUILD_DIR="$PROJECT_ROOT/build"
DOCS_DIR="$PROJECT_ROOT/docs"

# Configuration
DOXYGEN_CONFIG="$PROJECT_ROOT/Doxyfile.in"
BUILD_TYPE="${BUILD_TYPE:-Release}"

echo -e "${BLUE}FIX-FastTrade Documentation Generator${NC}"
echo "======================================"
echo "Project Root: $PROJECT_ROOT"
echo "Build Directory: $BUILD_DIR"
echo "Documentation Directory: $DOCS_DIR"
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

# Check if required tools are installed
check_tool() {
    if ! command -v "$1" &> /dev/null; then
        print_error "$1 is not installed. Please install $1 and try again."
        return 1
    fi
    return 0
}

# Check for required tools
print_status "Checking required tools..."

DOXYGEN_AVAILABLE=false
if check_tool doxygen; then
    DOXYGEN_AVAILABLE=true
    print_success "Doxygen found: $(doxygen --version)"
fi

CMAKE_AVAILABLE=false
if check_tool cmake; then
    CMAKE_AVAILABLE=true
    print_success "CMake found: $(cmake --version | head -n1)"
fi

echo ""

# Ensure build directory exists and is configured
if [ ! -d "$BUILD_DIR" ]; then
    print_warning "Build directory not found. Creating and configuring..."
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    if [ "$CMAKE_AVAILABLE" = true ]; then
        cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" "$PROJECT_ROOT"
    else
        print_error "CMake is required to configure the build"
        exit 1
    fi
else
    cd "$BUILD_DIR"
fi

# Generate documentation using CMake's Doxygen integration
if [ "$DOXYGEN_AVAILABLE" = true ] && [ "$CMAKE_AVAILABLE" = true ]; then
    print_status "Generating API documentation with Doxygen..."
    
    # Use CMake to generate documentation
    if cmake --build . --target docs --config "$BUILD_TYPE"; then
        print_success "Doxygen documentation generated successfully!"
        
        # Check if HTML documentation was created
        if [ -d "$BUILD_DIR/docs/html" ]; then
            print_success "HTML documentation available at: $BUILD_DIR/docs/html/index.html"
        fi
    else
        print_error "Failed to generate Doxygen documentation"
        exit 1
    fi
else
    print_warning "Skipping Doxygen documentation generation (tools not available)"
fi

echo ""

# Generate additional documentation files
print_status "Generating additional documentation..."

# Create docs directory if it doesn't exist
mkdir -p "$DOCS_DIR"

# Generate API overview
print_status "Creating API overview..."
cat > "$DOCS_DIR/api_overview.md" << 'EOF'
# FIX-FastTrade API Overview

This document provides an overview of the FIX-FastTrade API components.

## Core Components

### FIX Protocol Layer
- **Message**: Core FIX message handling
- **MessageParser**: FIX message parsing and validation
- **Session**: FIX session management
- **Config**: Configuration management

### Model Layer
- **Order**: Order representation and management
- **Trade**: Trade execution and tracking

### Service Layer
- **FixService**: FIX protocol service implementation
- **TradingService**: Trading logic and execution

### Infrastructure Layer
- **Memory Management**: High-performance memory pools
- **Threading**: CPU affinity and thread management
- **SIMD**: Vectorized operations for performance
- **Platform**: Cross-platform compatibility layer

## Usage Examples

See the individual API documentation files for detailed usage examples:
- [FIX API](api/fix_api.md)
- [Model API](api/model_api.md)
- [Service API](api/service_api.md)

## Performance Considerations

The FIX-FastTrade system is optimized for ultra-low latency:
- Zero-copy message handling where possible
- Memory pool allocation to avoid heap fragmentation
- SIMD optimizations for data processing
- CPU affinity for deterministic performance

For detailed performance characteristics, see the generated Doxygen documentation.
EOF

# Generate build information
print_status "Creating build information..."
cat > "$DOCS_DIR/build_info.md" << EOF
# Build Information

Generated: $(date)
Build Type: $BUILD_TYPE
CMake Version: $(cmake --version | head -n1 | cut -d' ' -f3)
Doxygen Version: $(doxygen --version 2>/dev/null || echo "Not available")

## Build Targets

- **fix-fasttrade**: Main executable
- **fix-fasttrade-core**: Static library for reuse
- **fix-fasttrade-tests**: Test suite
- **docs**: API documentation (Doxygen)

## Documentation Files

- API documentation: build/docs/html/index.html
- User guide: docs/user_guide.md
- Architecture: docs/design/architecture.md
- Installation: INSTALLATION.md

## Configuration

- Main config: config/fix-config.xml
- Examples: config/examples/
- Schema: config/schemas/fix-config.xsd
EOF

# Generate component dependency graph (if dot is available)
if command -v dot &> /dev/null; then
    print_status "Generating component dependency graph..."
    
    cat > "$BUILD_DIR/dependencies.dot" << 'EOF'
digraph FIXFastTrade {
    rankdir=TB;
    node [shape=box, style=filled, fillcolor=lightblue];
    
    // Applications
    "fix-fasttrade" [fillcolor=lightgreen];
    
    // Core Libraries
    "fix-fasttrade-core" [fillcolor=lightyellow];
    
    // Components
    "FixService" [fillcolor=lightcoral];
    "TradingService" [fillcolor=lightcoral];
    "Message" [fillcolor=lightgray];
    "MessageParser" [fillcolor=lightgray];
    "Session" [fillcolor=lightgray];
    "Config" [fillcolor=lightgray];
    "Order" [fillcolor=lightpink];
    "Trade" [fillcolor=lightpink];
    
    // Infrastructure
    "Memory" [fillcolor=wheat];
    "Threading" [fillcolor=wheat];
    "SIMD" [fillcolor=wheat];
    "Platform" [fillcolor=wheat];
    
    // Dependencies
    "fix-fasttrade" -> "fix-fasttrade-core";
    "fix-fasttrade-core" -> "FixService";
    "fix-fasttrade-core" -> "TradingService";
    "fix-fasttrade-core" -> "Message";
    "fix-fasttrade-core" -> "MessageParser";
    "fix-fasttrade-core" -> "Session";
    "fix-fasttrade-core" -> "Config";
    "fix-fasttrade-core" -> "Order";
    "fix-fasttrade-core" -> "Trade";
    
    "FixService" -> "Message";
    "FixService" -> "Session";
    "TradingService" -> "Order";
    "TradingService" -> "Trade";
    "MessageParser" -> "Message";
    "Session" -> "Config";
    
    // Infrastructure dependencies
    "Message" -> "Memory";
    "MessageParser" -> "SIMD";
    "Session" -> "Threading";
    "FixService" -> "Platform";
    "TradingService" -> "Platform";
}
EOF
    
    if dot -Tpng "$BUILD_DIR/dependencies.dot" -o "$DOCS_DIR/component_dependencies.png" 2>/dev/null; then
        print_success "Component dependency graph created: docs/component_dependencies.png"
    else
        print_warning "Failed to generate dependency graph"
    fi
else
    print_warning "Graphviz (dot) not available. Skipping dependency graph generation."
fi

# Create documentation index
print_status "Creating documentation index..."
cat > "$DOCS_DIR/index.md" << EOF
# FIX-FastTrade Documentation

Welcome to the FIX-FastTrade documentation.

## Quick Links

- [API Overview](api_overview.md)
- [Build Information](build_info.md)
- [User Guide](user_guide.md)
- [Installation Guide](../INSTALLATION.md)

## API Documentation

- [FIX API](api/fix_api.md) - FIX protocol implementation
- [Model API](api/model_api.md) - Order and Trade models
- [Service API](api/service_api.md) - Trading services

## Design Documentation

- [Architecture](design/architecture.md) - System architecture
- [FIX Protocol](design/fix_protocol.md) - FIX protocol implementation
- [Trading System](design/trading_system.md) - Trading system design

## Generated Documentation

$([ -d "$BUILD_DIR/docs/html" ] && echo "- [Doxygen API Reference](../build/docs/html/index.html) - Complete API reference" || echo "- Doxygen API Reference (not generated)")

## Configuration

- [Configuration Examples](../config/examples/) - Sample configurations
- [Configuration Schema](../config/schemas/fix-config.xsd) - XML schema

## Development

- [Building](../docs/building.md) - Build instructions
- [Testing](../tools/scripts/run_tests.sh) - Test execution
- [Configuration Validation](../tools/scripts/validate_config.sh) - Config validation

Generated: $(date)
EOF

# Copy important files to docs directory for easy access
print_status "Copying important documentation files..."

# Copy README if it exists
if [ -f "$PROJECT_ROOT/README.md" ]; then
    cp "$PROJECT_ROOT/README.md" "$DOCS_DIR/README_copy.md"
fi

# Copy installation guide if it exists
if [ -f "$PROJECT_ROOT/INSTALLATION.md" ]; then
    cp "$PROJECT_ROOT/INSTALLATION.md" "$DOCS_DIR/INSTALLATION_copy.md"
fi

print_success "Additional documentation files created"

# Summary
echo ""
echo "======================================"
print_success "Documentation generation completed!"
echo ""

print_status "Generated documentation:"
echo "  - API overview: docs/api_overview.md"
echo "  - Build info: docs/build_info.md"
echo "  - Documentation index: docs/index.md"

if [ -d "$BUILD_DIR/docs/html" ]; then
    echo "  - Doxygen HTML: build/docs/html/index.html"
fi

if [ -f "$DOCS_DIR/component_dependencies.png" ]; then
    echo "  - Dependency graph: docs/component_dependencies.png"
fi

echo ""
print_status "To view the documentation:"
echo "  - Open docs/index.md for the main index"

if [ -d "$BUILD_DIR/docs/html" ]; then
    echo "  - Open build/docs/html/index.html for full API reference"
fi

echo ""
print_status "Documentation is ready for use!"