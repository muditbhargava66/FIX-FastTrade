Build Configuration Examples
============================

This document provides comprehensive examples of CMake build configurations for different use cases, environments, and deployment scenarios.

Development Configurations
--------------------------

Standard Development Build
~~~~~~~~~~~~~~~~~~~~~~~~~~
Basic development configuration with debugging support.

.. code-block:: bash

#!/bin/bash
# dev-build.sh - Standard development build script

# Clean previous build
rm -rf build-dev
mkdir -p build-dev && cd build-dev

# Configure for development
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_INSTALL_PREFIX=$HOME/fix-fasttrade-dev \
    -DCMAKE_CXX_FLAGS="-Wall -Wextra -g3 -O0" \
    -DCMAKE_VERBOSE_MAKEFILE=ON

# Build with all cores
make -j$(nproc)

# Install to development directory
make install

echo "Development build complete. Executable: $HOME/fix-fasttrade-dev/bin/fix-fasttrade"


Debug Build with Sanitizers
~~~~~~~~~~~~~~~~~~~~~~~~~~~
Development build with memory and thread sanitizers.

.. code-block:: bash

#!/bin/bash
# debug-sanitizer-build.sh - Debug build with sanitizers

mkdir -p build-debug-sanitizer && cd build-debug-sanitizer

# Configure with sanitizers
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-fsanitize=address,thread,undefined -fno-omit-frame-pointer -g3" \
    -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,thread,undefined" \
    -DCMAKE_INSTALL_PREFIX=$HOME/fix-fasttrade-debug

make -j$(nproc)
make install

echo "Debug build with sanitizers complete."
echo "Run with: ASAN_OPTIONS=detect_leaks=1 ./bin/fix-fasttrade"


Profiling Build
~~~~~~~~~~~~~~~
Optimized build with profiling information.

.. code-block:: bash

#!/bin/bash
# profile-build.sh - Build for profiling

mkdir -p build-profile && cd build-profile

# Configure for profiling
cmake .. \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_CXX_FLAGS="-O2 -g -fno-omit-frame-pointer -pg" \
    -DCMAKE_EXE_LINKER_FLAGS="-pg" \
    -DCMAKE_INSTALL_PREFIX=$HOME/fix-fasttrade-profile

make -j$(nproc)
make install

echo "Profiling build complete."
echo "Run with profiling: ./bin/fix-fasttrade [args] && gprof ./bin/fix-fasttrade gmon.out > profile.txt"


Production Configurations
-------------------------

High-Performance Production Build
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Maximum performance build for production deployment.

.. code-block:: bash

#!/bin/bash
# production-build.sh - High-performance production build

mkdir -p build-production && cd build-production

# Detect CPU architecture
ARCH=$(uname -m)
if [[ "$ARCH" == "x86_64" ]]; then
    CPU_FLAGS="-march=native -mtune=native -mavx2 -mfma"
elif [[ "$ARCH" == "aarch64" ]] || [[ "$ARCH" == "arm64" ]]; then
    CPU_FLAGS="-march=native -mtune=native"
else
    CPU_FLAGS="-mtune=generic"
fi

# Configure for maximum performance
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O3 ${CPU_FLAGS} -flto -funroll-loops -ffast-math -DNDEBUG" \
    -DCMAKE_EXE_LINKER_FLAGS="-flto" \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
    -DCMAKE_INSTALL_PREFIX=/opt/fix-fasttrade

make -j$(nproc)

echo "High-performance production build complete."
echo "Install with: sudo make install"


Portable Production Build
~~~~~~~~~~~~~~~~~~~~~~~~~
Production build that works across different CPU architectures.

.. code-block:: bash

#!/bin/bash
# portable-build.sh - Portable production build

mkdir -p build-portable && cd build-portable

# Configure for portability
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O2 -mtune=generic -fno-fast-math" \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_SKIP_RPATH=ON

make -j$(nproc)

# Create portable package
make install DESTDIR=./package
tar -czf fix-fasttrade-portable.tar.gz -C package .

echo "Portable production build complete."
echo "Package: fix-fasttrade-portable.tar.gz"


Container-Optimized Build
~~~~~~~~~~~~~~~~~~~~~~~~~
Build optimized for containerized deployment.

.. code-block:: bash

#!/bin/bash
# container-build.sh - Container-optimized build

mkdir -p build-container && cd build-container

# Configure for container deployment
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O2 -mtune=generic -static-libgcc -static-libstdc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -static-libstdc++" \
    -DCMAKE_INSTALL_PREFIX=/app \
    -DBUILD_SHARED_LIBS=OFF

make -j$(nproc)
make install

# Create minimal container image
cat > Dockerfile << EOF
FROM ubuntu:20.04
RUN apt-get update && apt-get install -y libboost-system1.71.0 libboost-thread1.71.0 && rm -rf /var/lib/apt/lists/*
COPY --from=builder /app /app
EXPOSE 8080
USER 1000:1000
CMD ["/app/bin/fix-fasttrade", "--config", "/app/config/fix-config.xml"]
EOF

echo "Container-optimized build complete."


Platform-Specific Configurations
--------------------------------

macOS Apple Silicon Build
~~~~~~~~~~~~~~~~~~~~~~~~~
Optimized build for Apple Silicon (M1/M2/M3) processors.

.. code-block:: bash

#!/bin/bash
# macos-apple-silicon-build.sh - Apple Silicon optimized build

mkdir -p build-apple-silicon && cd build-apple-silicon

# Configure for Apple Silicon
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -mtune=native -flto" \
    -DCMAKE_EXE_LINKER_FLAGS="-flto" \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DBOOST_ROOT=$(brew --prefix boost) \
    -DQUICKFIX_ROOT=$(brew --prefix quickfix)

make -j$(sysctl -n hw.ncpu)
make install

echo "Apple Silicon build complete."
echo "Verify architecture: file /usr/local/bin/fix-fasttrade"


macOS Universal Binary Build
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Build that runs on both Intel and Apple Silicon Macs.

.. code-block:: bash

#!/bin/bash
# macos-universal-build.sh - Universal binary build

mkdir -p build-universal && cd build-universal

# Configure for universal binary
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 \
    -DCMAKE_CXX_FLAGS="-O2" \
    -DCMAKE_INSTALL_PREFIX=/usr/local

make -j$(sysctl -n hw.ncpu)

echo "Universal binary build complete."
echo "Verify: lipo -info bin/fix-fasttrade"


Linux ARM64 Build
~~~~~~~~~~~~~~~~~
Optimized build for ARM64 Linux systems (AWS Graviton, Raspberry Pi).

.. code-block:: bash

#!/bin/bash
# linux-arm64-build.sh - ARM64 Linux build

mkdir -p build-arm64 && cd build-arm64

# Configure for ARM64 Linux
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -mtune=native" \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_SYSTEM_PROCESSOR=aarch64

make -j$(nproc)

echo "ARM64 Linux build complete."
echo "Verify architecture: file bin/fix-fasttrade"


Windows MSVC Build
~~~~~~~~~~~~~~~~~~
Build configuration for Windows with Visual Studio.

.. code-block:: cmd

@echo off
REM windows-msvc-build.bat - Windows MSVC build

mkdir build-windows
cd build-windows

REM Configure for Windows with MSVC
cmake .. ^
    -G "Visual Studio 16 2019" ^
    -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_CXX_FLAGS="/O2 /arch:AVX2 /GL" ^
    -DCMAKE_EXE_LINKER_FLAGS="/LTCG" ^
    -DCMAKE_INSTALL_PREFIX="C:\Program Files\FIX-FastTrade" ^
    -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake

REM Build with all available cores
cmake --build . --config Release --parallel

echo Windows MSVC build complete.
echo Install with: cmake --build . --target install --config Release


Cross-Compilation Configurations
--------------------------------

Linux to ARM64 Cross-Compilation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Cross-compile from x86_64 Linux to ARM64.

.. code-block:: bash

#!/bin/bash
# cross-compile-arm64.sh - Cross-compile to ARM64

# Install cross-compilation toolchain
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

mkdir -p build-cross-arm64 && cd build-cross-arm64

# Create toolchain file
cat > arm64-toolchain.cmake << EOF
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF

# Configure for cross-compilation
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=arm64-toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O2 -march=armv8-a" \
    -DCMAKE_INSTALL_PREFIX=/opt/fix-fasttrade-arm64

make -j$(nproc)

echo "ARM64 cross-compilation complete."
echo "Verify: file bin/fix-fasttrade"


Docker Multi-Architecture Build
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Build for multiple architectures using Docker buildx.

.. code-block:: dockerfile

# Dockerfile.multiarch - Multi-architecture Docker build
FROM --platform=$BUILDPLATFORM ubuntu:20.04 AS builder

ARG TARGETPLATFORM
ARG BUILDPLATFORM

RUN apt-get update && apt-get install -y \
    build-essential cmake git \
    libboost-all-dev pkg-config

COPY . /src
WORKDIR /src/build

# Configure based on target platform
RUN if [ "$TARGETPLATFORM" = "linux/amd64" ]; then \
        cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=x86-64"; \
    elif [ "$TARGETPLATFORM" = "linux/arm64" ]; then \
        cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=armv8-a"; \
    else \
        cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O2"; \
    fi

RUN make -j$(nproc) && make install DESTDIR=/app

FROM ubuntu:20.04
RUN apt-get update && apt-get install -y libboost-system1.71.0 libboost-thread1.71.0
COPY --from=builder /app/usr/local /usr/local
CMD ["/usr/local/bin/fix-fasttrade"]


.. code-block:: bash

# Build for multiple architectures
docker buildx build --platform linux/amd64,linux/arm64 -t fix-fasttrade:latest .


Specialized Configurations
--------------------------

Static Linking Build
~~~~~~~~~~~~~~~~~~~~
Build with all dependencies statically linked.

.. code-block:: bash

#!/bin/bash
# static-build.sh - Static linking build

mkdir -p build-static && cd build-static

# Configure for static linking
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O2 -static-libgcc -static-libstdc++" \
    -DCMAKE_EXE_LINKER_FLAGS="-static -static-libgcc -static-libstdc++" \
    -DBUILD_SHARED_LIBS=OFF \
    -DBoost_USE_STATIC_LIBS=ON \
    -DCMAKE_INSTALL_PREFIX=/opt/fix-fasttrade-static

make -j$(nproc)

echo "Static linking build complete."
echo "Check dependencies: ldd bin/fix-fasttrade"


Minimal Size Build
~~~~~~~~~~~~~~~~~~
Build optimized for minimal binary size.

.. code-block:: bash

#!/bin/bash
# minimal-size-build.sh - Minimal size build

mkdir -p build-minimal && cd build-minimal

# Configure for minimal size
cmake .. \
    -DCMAKE_BUILD_TYPE=MinSizeRel \
    -DCMAKE_CXX_FLAGS="-Os -ffunction-sections -fdata-sections" \
    -DCMAKE_EXE_LINKER_FLAGS="-Wl,--gc-sections -s" \
    -DCMAKE_INSTALL_PREFIX=/opt/fix-fasttrade-minimal

make -j$(nproc)

# Strip additional symbols
strip --strip-all bin/fix-fasttrade

echo "Minimal size build complete."
echo "Binary size: $(du -h bin/fix-fasttrade)"


Security-Hardened Build
~~~~~~~~~~~~~~~~~~~~~~~
Build with security hardening features.

.. code-block:: bash

#!/bin/bash
# security-hardened-build.sh - Security hardened build

mkdir -p build-hardened && cd build-hardened

# Configure with security hardening
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O2 -D_FORTIFY_SOURCE=2 -fstack-protector-strong -fPIE" \
    -DCMAKE_EXE_LINKER_FLAGS="-pie -Wl,-z,relro -Wl,-z,now" \
    -DCMAKE_INSTALL_PREFIX=/opt/fix-fasttrade-hardened

make -j$(nproc)

echo "Security-hardened build complete."
echo "Check security features: checksec --file=bin/fix-fasttrade"


Testing and Validation Configurations
-------------------------------------

Unit Testing Build
~~~~~~~~~~~~~~~~~~
Build specifically for running unit tests.

.. code-block:: bash

#!/bin/bash
# test-build.sh - Unit testing build

mkdir -p build-test && cd build-test

# Configure for testing
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-g3 -O0 --coverage" \
    -DCMAKE_EXE_LINKER_FLAGS="--coverage" \
    -DCMAKE_INSTALL_PREFIX=$HOME/fix-fasttrade-test

make -j$(nproc)

# Run tests
make test

# Generate coverage report
gcov -r .
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage-report

echo "Unit testing build complete."
echo "Coverage report: coverage-report/index.html"


Benchmark Build
~~~~~~~~~~~~~~~
Build optimized for performance benchmarking.

.. code-block:: bash

#!/bin/bash
# benchmark-build.sh - Benchmark build

mkdir -p build-benchmark && cd build-benchmark

# Configure for benchmarking
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -DNDEBUG -fno-omit-frame-pointer" \
    -DCMAKE_INSTALL_PREFIX=$HOME/fix-fasttrade-benchmark \
    -DBUILD_BENCHMARKS=ON

make -j$(nproc)

echo "Benchmark build complete."
echo "Run benchmarks: ./bin/fix-fasttrade-benchmarks"


Continuous Integration Build
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Build configuration for CI/CD pipelines.

.. code-block:: bash

#!/bin/bash
# ci-build.sh - Continuous Integration build

set -e  # Exit on any error

# Clean environment
rm -rf build-ci
mkdir -p build-ci && cd build-ci

# Configure for CI
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O2 -Werror -Wall -Wextra" \
    -DCMAKE_INSTALL_PREFIX=/tmp/fix-fasttrade-ci \
    -DCMAKE_VERBOSE_MAKEFILE=ON

# Build with limited parallelism (CI resource constraints)
make -j2

# Run tests
ctest --output-on-failure

# Install and package
make install
cpack

echo "CI build complete."
echo "Artifacts: $(ls -la *.tar.gz *.deb *.rpm 2>/dev/null || echo 'No packages created')"


Build Automation Scripts
------------------------

Master Build Script
~~~~~~~~~~~~~~~~~~~
Comprehensive build script that handles multiple configurations.

.. code-block:: bash

#!/bin/bash
# master-build.sh - Master build script

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Default values
BUILD_TYPE="Release"
INSTALL_PREFIX="/usr/local"
PARALLEL_JOBS=$(nproc)
CLEAN_BUILD=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --install-prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --jobs)
            PARALLEL_JOBS="$2"
            shift 2
            ;;
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  --build-type TYPE     Build type (Debug, Release, RelWithDebInfo, MinSizeRel)"
            echo "  --install-prefix DIR  Installation prefix"
            echo "  --jobs N              Number of parallel jobs"
            echo "  --clean               Clean build directory first"
            echo "  --help                Show this help"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Build directory
BUILD_DIR="$PROJECT_ROOT/build-${BUILD_TYPE,,}"

# Clean if requested
if [[ "$CLEAN_BUILD" == true ]]; then
    echo "Cleaning build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
echo "Configuring build..."
cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build
echo "Building with $PARALLEL_JOBS parallel jobs..."
make -j"$PARALLEL_JOBS"

# Test
echo "Running tests..."
ctest --output-on-failure

# Install
echo "Installing to $INSTALL_PREFIX..."
make install

echo "Build complete!"
echo "Build type: $BUILD_TYPE"
echo "Install prefix: $INSTALL_PREFIX"
echo "Executable: $INSTALL_PREFIX/bin/fix-fasttrade"


This comprehensive collection of build configuration examples provides templates for virtually any build scenario, from development to production deployment across different platforms and use cases.