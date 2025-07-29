Installation Guide
==================

This guide provides step-by-step instructions to install and build the **FUNCTIONAL** FIX-FastTrade system. The project has been successfully tested and builds on macOS (ARM64/Intel), Linux, and Windows platforms.

🚀 Quick Start (Recommended)
---------------------------

Option 1: Automated Setup Script (Recommended)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: bash

# Clone the repository with all submodules
git clone --recursive https://github.com/muditbhargava66/FIX-FastTrade.git
cd FIX-FastTrade

# Run the automated setup script (installs dependencies)
./scripts/setup.sh

# Build the project
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)  # Linux: nproc, macOS: sysctl -n hw.ncpu

# Verify the build
./bin/fix-fasttrade --help


Option 2: Manual Build Process
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: bash

# Clone with submodules
git clone --recursive https://github.com/muditbhargava66/FIX-FastTrade.git
cd FIX-FastTrade

# If you forgot --recursive, initialize submodules
git submodule update --init --recursive

# Create build directory and configure
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with optimal performance
make -j$(nproc)  # Use $(sysctl -n hw.ncpu) on macOS

# Run tests to verify build
make test


📦 Cross-Platform Package Manager Options
----------------------------------------

Using vcpkg (Windows/Linux/macOS)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Linux/macOS:**
.. code-block:: bash

# Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh

# Install dependencies for Linux
./vcpkg install boost:x64-linux quickfix:x64-linux tbb:x64-linux benchmark:x64-linux

# Install dependencies for macOS Intel
./vcpkg install boost:x64-osx quickfix:x64-osx tbb:x64-osx benchmark:x64-osx

# Install dependencies for macOS Apple Silicon
./vcpkg install boost:arm64-osx quickfix:arm64-osx tbb:arm64-osx benchmark:arm64-osx

# Build with vcpkg
cd /path/to/FIX-FastTrade
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
make -j$(nproc)  # Linux: nproc, macOS: sysctl -n hw.ncpu


**Windows:**
.. code-block:: cmd

REM Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

REM Install dependencies
.\vcpkg install boost:x64-windows quickfix:x64-windows tbb:x64-windows benchmark:x64-windows

REM Build with vcpkg
cd \path\to\FIX-FastTrade
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build . --config Release --parallel


Using Conan
~~~~~~~~~~~
.. code-block:: bash

# Install Conan
pip install conan

# Install dependencies
conan install . --build=missing

# Build
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
make -j$(nproc)


Using System Package Managers
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

macOS (Homebrew) - ARM64 & Intel Support
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Install required dependencies
brew install cmake boost quickfix pkg-config

# Optional performance dependencies
brew install intel-tbb google-benchmark spdlog

# Clone and build
git clone --recursive https://github.com/muditbhargava66/FIX-FastTrade.git
cd FIX-FastTrade
mkdir -p build && cd build

# Configure for your architecture
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)

# Test the build
./bin/fix-fasttrade --help


Ubuntu/Debian
^^^^^^^^^^^^^
.. code-block:: bash

# Update package list
sudo apt update

# Install required dependencies
sudo apt install -y \
    build-essential cmake pkg-config \
    libboost-all-dev \
    libquickfix-dev

# Optional performance dependencies
sudo apt install -y \
    libtbb-dev \
    libbenchmark-dev \
    libspdlog-dev

# Clone and build
git clone --recursive https://github.com/muditbhargava66/FIX-FastTrade.git
cd FIX-FastTrade
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Verify installation
./bin/fix-fasttrade --help


CentOS/RHEL/Fedora
^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# CentOS/RHEL
sudo yum install -y gcc-c++ cmake pkgconfig boost-devel

# Fedora
sudo dnf install -y gcc-c++ cmake pkgconfig boost-devel

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)


🔧 Build Configuration Options
-----------------------------

Debug Build
~~~~~~~~~~~
.. code-block:: bash

cmake .. -DCMAKE_BUILD_TYPE=Debug


Release Build (Optimized)
~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: bash

cmake .. -DCMAKE_BUILD_TYPE=Release


With Specific Features
~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: bash

# Enable all optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_SIMD=ON -DENABLE_BENCHMARKS=ON

# Disable specific features
cmake .. -DENABLE_TBB=OFF -DENABLE_BENCHMARKS=OFF


🧪 Testing
---------

Run Unit Tests
~~~~~~~~~~~~~~
.. code-block:: bash

cd build

# Run all tests
make test

# Run tests with detailed output
ctest --verbose

# Run specific test categories
ctest -R "fix.*" --verbose    # FIX protocol tests
ctest -R "model.*" --verbose  # Trading model tests


Run Performance Benchmarks (if enabled)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: bash

cd build

# Run performance benchmarks
./bin/fix-fasttrade-benchmarks

# Run with specific parameters
./bin/fix-fasttrade --config ../config/fix-config.xml --stats-interval 10


Verify Installation
~~~~~~~~~~~~~~~~~~~
.. code-block:: bash

# Check executable works
./bin/fix-fasttrade --help

# Test with sample configuration
./bin/fix-fasttrade --config ../config/fix-config.xml --cpu-main 0


🐳 Docker Support
----------------

Build Docker Image
~~~~~~~~~~~~~~~~~~
.. code-block:: bash

docker build -t fix-fasttrade .


Run in Container
~~~~~~~~~~~~~~~~
.. code-block:: bash

docker run -it fix-fasttrade


📋 Dependencies
--------------

Required Dependencies (Verified Working)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* **CMake** >= 3.14 ✅
* **C++17** compatible compiler (GCC 8+, Clang 7+, MSVC 2019+) ✅
* **Boost** >= 1.71.0 (system, thread, filesystem, date_time, program_options, log) ✅
* **QuickFIX** (included as git submodule) ✅
* **GoogleTest** (included as git submodule) ✅

Optional Dependencies (Performance Enhancements)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* **Intel TBB** (parallel algorithms) ⚡
* **Google Benchmark** (performance testing) 📊
* **spdlog** (high-performance logging) 📝
* **Doxygen** (API documentation generation) 📚

Platform-Specific Notes
~~~~~~~~~~~~~~~~~~~~~~~

macOS Support
^^^^^^^^^^^^^
* **Apple Silicon (ARM64)**: Native M1/M2 support with NEON SIMD optimizations
  - Minimum: macOS 11.0 (Big Sur)
  - Optimized for Apple's unified memory architecture
  - Native performance without Rosetta translation
* **Intel (x86_64)**: Full Intel optimization with SSE/AVX support
  - Minimum: macOS 10.15 (Catalina)
  - AVX2 and Intel TBB optimizations available
  - Supports all Intel Mac models from 2015+

Linux Support
^^^^^^^^^^^^^
* **x86_64**: Comprehensive support across distributions
  - Ubuntu 20.04+, Debian 11+, CentOS 8+, RHEL 8+, Fedora 34+
  - SSE4.2, AVX, AVX2, and AVX-512 SIMD support
  - Intel TBB and custom memory allocators
* **ARM64**: Native ARM support for modern ARM processors
  - Raspberry Pi 4+, AWS Graviton2/3, Ampere Altra
  - NEON SIMD optimizations
  - Tested on Ubuntu 20.04+ ARM64

Windows Support
^^^^^^^^^^^^^^^
* **x86_64**: Full Windows 10/11 support
  - MSVC 2019+ with C++17 support
  - vcpkg recommended for dependency management
  - SSE/AVX optimizations with Intel TBB
* **ARM64**: Experimental Windows on ARM support
  - Windows 11 ARM64 with native compilation
  - Limited testing on Surface Pro X and similar devices

🔍 Troubleshooting
-----------------

Common Issues
~~~~~~~~~~~~~

1. QuickFIX Not Found
^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# macOS
brew install quickfix

# Ubuntu
sudo apt install libquickfix-dev

# Or build from source
cd third_party/quickfix
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)


2. Boost Version Issues
^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Check Boost version
cmake .. -DBoost_DEBUG=ON

# Force specific Boost version
cmake .. -DBOOST_ROOT=/path/to/boost


3. Compiler Issues
^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Specify compiler
cmake .. -DCMAKE_CXX_COMPILER=g++-9

# Check C++17 support
g++ --version
clang++ --version


4. Platform-Specific Issues
^^^^^^^^^^^^^^^^^^^^^^^^^^^

**macOS ARM64 (Apple Silicon):**
.. code-block:: bash

# Use Rosetta if needed
arch -x86_64 brew install quickfix

# Native ARM build
cmake .. -DCMAKE_OSX_ARCHITECTURES=arm64


**Linux Missing Libraries:**
.. code-block:: bash

# Install development packages
sudo apt install build-essential libc6-dev

# Check library paths
ldconfig -p | grep boost


🚀 Performance Optimization
--------------------------

For Maximum Performance
~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: bash

cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -mtune=native" \
    -DENABLE_SIMD=ON \
    -DENABLE_LTO=ON


For Development
~~~~~~~~~~~~~~~
.. code-block:: bash

cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_SANITIZERS=ON \
    -DENABLE_TESTING=ON


📚 Next Steps
------------

After successful installation:

1. **Configure**: Edit ``config/fix-config.xml``
2. **Run**: ``./bin/fix-fasttrade --config config/fix-config.xml``
3. **Test**: ``make test``
4. **Benchmark**: ``./bin/fix-fasttrade-benchmarks``

🆘 Getting Help
--------------

* **Issues**: [GitHub Issues](https://github.com/muditbhargava66/FIX-FastTrade/issues)
* **Discussions**: [GitHub Discussions](https://github.com/muditbhargava66/FIX-FastTrade/discussions)
* **Documentation**: [Project Wiki](https://github.com/muditbhargava66/FIX-FastTrade/wiki)