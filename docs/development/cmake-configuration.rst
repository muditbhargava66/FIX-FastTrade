# CMake Configuration Guide

This document provides comprehensive information about the CMake build system configuration for FIX-FastTrade, including available targets, build options, packaging configuration, and installation targets.

CMake Targets
-------------

The FIX-FastTrade project provides several CMake targets for different use cases:

Primary Targets
~~~~~~~~~~~~~~~

``fix-fasttrade`` (Executable)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The main trading system executable with full optimizations.

.. code-block:: bash

# Build main executable only
make fix-fasttrade

# Or with CMake directly
cmake --build . --target fix-fasttrade


**Target Properties:**
* **Type**: Executable
* **Output**: ``${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fix-fasttrade``
* **Sources**: All infrastructure, FIX, model, service sources + main.cpp
* **Dependencies**: Boost, QuickFIX, Threads, TBB (optional), spdlog (optional)

``fix-fasttrade-core`` (Static Library)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Core functionality as a static library for integration with other projects.

.. code-block:: bash

# Build static library only
make fix-fasttrade-core

# Or with CMake directly
cmake --build . --target fix-fasttrade-core


**Target Properties:**
* **Type**: Static Library
* **Output**: ``${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/libfix-fasttrade-core.a``
* **Sources**: Infrastructure, FIX, model, service sources (excluding main.cpp)
* **Public Dependencies**: Boost, QuickFIX, Threads, TBB (optional)

Test Targets
~~~~~~~~~~~~

``fix-fasttrade-tests`` (Test Executable)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Comprehensive test suite using Google Test framework.

.. code-block:: bash

# Build and run tests
make test

# Build test executable only
make fix-fasttrade-tests

# Run tests with verbose output
ctest --verbose


**Target Properties:**
* **Type**: Executable
* **Output**: ``${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fix-fasttrade-tests``
* **Sources**: All test files from ``tests/`` directory
* **Dependencies**: Google Test, fix-fasttrade-core

Documentation Targets
~~~~~~~~~~~~~~~~~~~~~

``docs`` (Documentation Generation)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Generate API documentation using Doxygen (requires Doxygen to be installed).

.. code-block:: bash

# Generate documentation
make docs

# Or with CMake directly
cmake --build . --target docs


**Target Properties:**
* **Type**: Custom Target
* **Output**: ``${CMAKE_BINARY_DIR}/docs/html/``
* **Requirements**: Doxygen must be found by CMake
* **Sources**: All header files in ``include/``, source files in ``src/``, and README.md

Utility Targets
~~~~~~~~~~~~~~~

``clean``
^^^^^^^^^
Remove all build artifacts.

.. code-block:: bash

make clean


``install``
^^^^^^^^^^^
Install the built targets to the system.

.. code-block:: bash

make install
# Or with CMake directly
cmake --build . --target install


Build Options and Variables
---------------------------

Standard CMake Variables
~~~~~~~~~~~~~~~~~~~~~~~~

``CMAKE_BUILD_TYPE``
^^^^^^^^^^^^^^^^^^^^
Controls the build configuration and optimization level.

.. code-block:: bash

# Debug build (default for development)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release build (optimized for production)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Release with debug info
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Minimum size release
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel


**Build Type Effects:**
* **Debug**: ``-O0 -g3 -DDEBUG -fsanitize=address -fno-omit-frame-pointer``
* **Release**: ``-O3 -march=native -mtune=native -flto -funroll-loops -ffast-math -DNDEBUG``
* **RelWithDebInfo**: Release optimizations + debug symbols
* **MinSizeRel**: Size-optimized release build

``CMAKE_CXX_STANDARD``
^^^^^^^^^^^^^^^^^^^^^^
C++ standard version (fixed to C++17 for this project).

.. code-block:: bash

# This is set automatically, but can be overridden
cmake .. -DCMAKE_CXX_STANDARD=20


``CMAKE_INSTALL_PREFIX``
^^^^^^^^^^^^^^^^^^^^^^^^
Installation directory prefix.

.. code-block:: bash

# Install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/fix-fasttrade

# Install to user directory
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/fix-fasttrade


Output Directory Variables
~~~~~~~~~~~~~~~~~~~~~~~~~~

``CMAKE_RUNTIME_OUTPUT_DIRECTORY``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Directory for executable files (default: ``${CMAKE_BINARY_DIR}/bin``).

.. code-block:: bash

cmake .. -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=/custom/bin/path


``CMAKE_LIBRARY_OUTPUT_DIRECTORY``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Directory for shared library files (default: ``${CMAKE_BINARY_DIR}/lib``).

.. code-block:: bash

cmake .. -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=/custom/lib/path


``CMAKE_ARCHIVE_OUTPUT_DIRECTORY``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Directory for static library files (default: ``${CMAKE_BINARY_DIR}/lib``).

.. code-block:: bash

cmake .. -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=/custom/lib/path


Compiler-Specific Options
~~~~~~~~~~~~~~~~~~~~~~~~~

GCC/Clang Options
^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Custom compiler flags
cmake .. -DCMAKE_CXX_FLAGS="-Wall -Wextra -O2"

# Link-time optimization
cmake .. -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON

# Address sanitizer (Debug builds)
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-fsanitize=address"


MSVC Options
^^^^^^^^^^^^
.. code-block:: bash

# Custom MSVC flags
cmake .. -DCMAKE_CXX_FLAGS="/W4 /O2"

# Multi-processor compilation
cmake .. -DCMAKE_CXX_FLAGS="/MP"


Platform-Specific Options
~~~~~~~~~~~~~~~~~~~~~~~~~

macOS Options
^^^^^^^^^^^^^
.. code-block:: bash

# Target specific macOS version
cmake .. -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0

# Target specific architecture
cmake .. -DCMAKE_OSX_ARCHITECTURES=arm64

# Universal binary (Intel + Apple Silicon)
cmake .. -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"


Windows Options
^^^^^^^^^^^^^^^
.. code-block:: bash

# Target specific Windows version
cmake .. -DCMAKE_SYSTEM_VERSION=10.0

# Use specific Visual Studio generator
cmake .. -G "Visual Studio 16 2019" -A x64


Dependency Configuration
------------------------

Boost Configuration
~~~~~~~~~~~~~~~~~~~
.. code-block:: bash

# Specify Boost root directory
cmake .. -DBOOST_ROOT=/usr/local/boost

# Use specific Boost version
cmake .. -DBoost_NO_SYSTEM_PATHS=ON -DBOOST_ROOT=/opt/boost_1_82_0

# Disable specific Boost components
cmake .. -DBoost_NO_BOOST_CMAKE=ON


QuickFIX Configuration
~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: bash

# Specify QuickFIX installation path
cmake .. -DQUICKFIX_ROOT=/usr/local/quickfix

# Use QuickFIX from submodule
cmake .. -DQUICKFIX_INCLUDE_DIR=third_party/quickfix/include \
         -DQUICKFIX_LIBRARY=third_party/quickfix/lib/libquickfix.a


Optional Dependencies
~~~~~~~~~~~~~~~~~~~~~
.. code-block:: bash

# Enable Intel TBB
cmake .. -DTBB_ROOT=/opt/intel/tbb

# Enable Google Benchmark
cmake .. -Dbenchmark_ROOT=/usr/local/benchmark

# Enable spdlog
cmake .. -Dspdlog_ROOT=/usr/local/spdlog


Build Configuration Examples
----------------------------

Development Build
~~~~~~~~~~~~~~~~~
Optimized for fast compilation and debugging.

.. code-block:: bash

mkdir -p build-debug && cd build-debug
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_CXX_FLAGS="-Wall -Wextra -g3"
make -j$(nproc)


Production Release Build
~~~~~~~~~~~~~~~~~~~~~~~~
Fully optimized for maximum performance.

.. code-block:: bash

mkdir -p build-release && cd build-release
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -mtune=native -flto"
make -j$(nproc)


Cross-Platform Release Build
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Optimized but portable across different CPUs.

.. code-block:: bash

mkdir -p build-portable && cd build-portable
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O2 -mtune=generic"
make -j$(nproc)


Profiling Build
~~~~~~~~~~~~~~~
Optimized with profiling information.

.. code-block:: bash

mkdir -p build-profile && cd build-profile
cmake .. \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_CXX_FLAGS="-O2 -g -fno-omit-frame-pointer"
make -j$(nproc)


Static Analysis Build
~~~~~~~~~~~~~~~~~~~~~
Build with static analysis tools enabled.

.. code-block:: bash

mkdir -p build-analysis && cd build-analysis
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_CLANG_TIDY="clang-tidy;-checks=*" \
    -DCMAKE_CXX_CPPCHECK="cppcheck;--enable=all"
make -j$(nproc)


Sanitizer Builds
~~~~~~~~~~~~~~~~
Builds with various sanitizers for debugging.

.. code-block:: bash

# Address Sanitizer
mkdir -p build-asan && cd build-asan
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer"

# Thread Sanitizer
mkdir -p build-tsan && cd build-tsan
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-fsanitize=thread"

# Memory Sanitizer
mkdir -p build-msan && cd build-msan
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-fsanitize=memory"


Advanced Build Configurations
-----------------------------

Custom Toolchain Build
~~~~~~~~~~~~~~~~~~~~~~
Using a specific compiler toolchain.

.. code-block:: bash

# Create toolchain file (toolchain-gcc11.cmake)
cat > toolchain-gcc11.cmake << EOF
set(CMAKE_C_COMPILER gcc-11)
set(CMAKE_CXX_COMPILER g++-11)
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra")
EOF

# Use custom toolchain
cmake .. -DCMAKE_TOOLCHAIN_FILE=toolchain-gcc11.cmake


Ninja Build System
~~~~~~~~~~~~~~~~~~
Using Ninja instead of Make for faster builds.

.. code-block:: bash

# Generate Ninja build files
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build with Ninja
ninja -j$(nproc)

# Build specific target
ninja fix-fasttrade


Multi-Configuration Build
~~~~~~~~~~~~~~~~~~~~~~~~~
Using Visual Studio or Xcode generators.

.. code-block:: bash

# Visual Studio (Windows)
cmake .. -G "Visual Studio 16 2019" -A x64

# Build Release configuration
cmake --build . --config Release

# Build Debug configuration
cmake --build . --config Debug

# Xcode (macOS)
cmake .. -G Xcode
cmake --build . --config Release


Installation Configuration
--------------------------

Install Targets
~~~~~~~~~~~~~~~

The CMake configuration defines several install targets:

Executables
^^^^^^^^^^^
.. code-block:: cmake

install(TARGETS fix-fasttrade fix-fasttrade-core
    RUNTIME DESTINATION bin      # Executables
    LIBRARY DESTINATION lib      # Shared libraries
    ARCHIVE DESTINATION lib      # Static libraries
)


Header Files
^^^^^^^^^^^^
.. code-block:: cmake

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ 
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)


Configuration Files
^^^^^^^^^^^^^^^^^^^
.. code-block:: cmake

install(DIRECTORY ${CMAKE_SOURCE_DIR}/config/ 
    DESTINATION config
    FILES_MATCHING PATTERN "*.json" PATTERN "*.xml" PATTERN "*.conf"
)


Documentation
^^^^^^^^^^^^^
.. code-block:: cmake

install(DIRECTORY ${CMAKE_BINARY_DIR}/docs/html/ 
    DESTINATION docs
    OPTIONAL
)


Installation Examples
~~~~~~~~~~~~~~~~~~~~~

System-Wide Installation
^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Build and install to /usr/local
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make -j$(nproc)
sudo make install


User Installation
^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Install to user directory
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
make -j$(nproc)
make install


Custom Installation
^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Install to custom directory
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/fix-fasttrade
make -j$(nproc)
sudo make install


Component-Specific Installation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Install only runtime components
cmake --build . --target install --component Runtime

# Install only development components
cmake --build . --target install --component Development


Packaging Configuration (CPack)
-------------------------------

FIX-FastTrade includes comprehensive CPack configuration for creating distribution packages.

Package Generators
~~~~~~~~~~~~~~~~~~

The following package formats are supported:

* **ZIP**: Cross-platform archive
* **TGZ**: Compressed tar archive
* **DEB**: Debian/Ubuntu packages
* **RPM**: Red Hat/CentOS/Fedora packages

Package Configuration Variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

set(CPACK_GENERATOR "ZIP;TGZ;DEB;RPM")
set(CPACK_PACKAGE_NAME "FIX-FastTrade")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "Ultra-low latency FIX protocol trading system")
set(CPACK_PACKAGE_CONTACT "FIX-FastTrade Team")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")


Debian Package Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-all-dev (>= 1.71)")
set(CPACK_DEBIAN_PACKAGE_SECTION "finance")


RPM Package Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

set(CPACK_RPM_PACKAGE_REQUIRES "boost >= 1.71")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Finance")


Creating Packages
~~~~~~~~~~~~~~~~~

Create All Package Types
^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Build the project first
make -j$(nproc)

# Create packages
cpack


Create Specific Package Types
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Create only DEB package
cpack -G DEB

# Create only RPM package
cpack -G RPM

# Create only ZIP archive
cpack -G ZIP


Custom Package Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Override package name
cpack -D CPACK_PACKAGE_NAME="CustomFIXTrade"

# Override version
cpack -D CPACK_PACKAGE_VERSION="2.1.0"

# Create package in specific directory
cpack --config CPackConfig.cmake -B /tmp/packages


Package Contents
~~~~~~~~~~~~~~~~

Each package includes:

* **Executables**: ``fix-fasttrade`` binary
* **Libraries**: ``libfix-fasttrade-core.a`` static library
* **Headers**: All public header files from ``include/``
* **Configuration**: Example configuration files from ``config/``
* **Documentation**: Generated API documentation (if available)
* **License**: Project license file
* **README**: Project documentation

Doxygen Configuration
---------------------

Doxygen Integration
~~~~~~~~~~~~~~~~~~~

The CMake configuration automatically integrates with Doxygen for API documentation generation.

Doxygen Variables
^^^^^^^^^^^^^^^^^
.. code-block:: cmake

set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_GENERATE_LATEX NO)
set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_EXTRACT_PRIVATE YES)
set(DOXYGEN_EXTRACT_STATIC YES)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_SOURCE_DIR}/README.md)
set(DOXYGEN_EXCLUDE_PATTERNS */third_party/* */build/*)


Generating Documentation
^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Generate documentation (requires Doxygen)
make docs

# Or with CMake directly
cmake --build . --target docs

# View generated documentation
open build/docs/html/index.html  # macOS
xdg-open build/docs/html/index.html  # Linux


Custom Doxygen Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Use custom Doxyfile
cmake .. -DDOXYGEN_CONFIG_FILE=/path/to/custom/Doxyfile

# Override output directory
cmake .. -DDOXYGEN_OUTPUT_DIRECTORY=/custom/docs/path


Configuration Summary
---------------------

Build Information Display
~~~~~~~~~~~~~~~~~~~~~~~~~

The CMake configuration displays a comprehensive summary after configuration:


FIX-FastTrade Configuration Summary:
  Version: 2.0.0
  Build Type: Release
  C++ Standard: 17
  Compiler: GNU 11.3.0
  Boost Version: 1.82.0
  QuickFIX: Found at /usr/local/include
  Intel TBB: TRUE
  Google Benchmark: TRUE
  Documentation: TRUE


Verification Commands
~~~~~~~~~~~~~~~~~~~~~

After configuration, verify the setup:

.. code-block:: bash

# Check available targets
cmake --build . --target help

# Verify compiler flags
cmake -LA | grep CMAKE_CXX_FLAGS

# Check install destinations
cmake -LA | grep CMAKE_INSTALL

# Verify dependencies
cmake -LA | grep -E "(Boost|QUICKFIX|TBB)"


Troubleshooting CMake Issues
----------------------------

Common Configuration Problems
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Boost Not Found
^^^^^^^^^^^^^^^
.. code-block:: bash

# Solution 1: Specify Boost root
cmake .. -DBOOST_ROOT=/usr/local/boost

# Solution 2: Use pkg-config
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
cmake ..

# Solution 3: Install Boost development packages
sudo apt install libboost-all-dev  # Ubuntu/Debian
brew install boost                  # macOS


QuickFIX Not Found
^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Solution 1: Initialize submodules
git submodule update --init --recursive

# Solution 2: Specify QuickFIX paths
cmake .. -DQUICKFIX_INCLUDE_DIR=/usr/local/include \
         -DQUICKFIX_LIBRARY=/usr/local/lib/libquickfix.a

# Solution 3: Build QuickFIX submodule manually
cd third_party/quickfix
mkdir build && cd build
cmake .. && make -j$(nproc)


Compiler Version Issues
^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Check compiler version
gcc --version
clang --version

# Use specific compiler
cmake .. -DCMAKE_C_COMPILER=gcc-11 -DCMAKE_CXX_COMPILER=g++-11

# Update compiler (Ubuntu)
sudo apt install gcc-11 g++-11
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 100


CMake Version Issues
^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Check CMake version
cmake --version

# Update CMake (Ubuntu)
sudo apt remove cmake
pip install cmake

# Update CMake (macOS)
brew upgrade cmake


This comprehensive CMake configuration guide provides all the information needed to build, configure, and package FIX-FastTrade across different platforms and use cases.