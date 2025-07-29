# Packaging Guide

This document describes how to create distribution packages for FIX-FastTrade using CPack, including configuration options, supported package formats, and platform-specific considerations.

Overview
--------

FIX-FastTrade uses CPack (part of CMake) to create distribution packages in various formats. The packaging system is configured to create professional, installable packages for different operating systems and package managers.

Supported Package Formats
-------------------------

Archive Formats
~~~~~~~~~~~~~~~

ZIP Archives
^^^^^^^^^^^^
Cross-platform compressed archives suitable for manual installation.

.. code-block:: bash

# Create ZIP package
cpack -G ZIP

# Output: FIX-FastTrade-2.0.0-Linux.zip (or similar)


**Use Cases:**
* Manual installation on any platform
* Distribution via web download
* Development and testing

TGZ Archives
^^^^^^^^^^^^
Compressed tar archives, standard on Unix-like systems.

.. code-block:: bash

# Create TGZ package
cpack -G TGZ

# Output: FIX-FastTrade-2.0.0-Linux.tar.gz


**Use Cases:**
* Unix/Linux manual installation
* Source distribution
* Backup and archival

Linux Package Formats
~~~~~~~~~~~~~~~~~~~~~

DEB Packages (Debian/Ubuntu)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Native Debian package format for Debian, Ubuntu, and derivatives.

.. code-block:: bash

# Create DEB package
cpack -G DEB

# Output: FIX-FastTrade-2.0.0-Linux.deb


**Installation:**
.. code-block:: bash

# Install the package
sudo dpkg -i FIX-FastTrade-2.0.0-Linux.deb

# Install dependencies if needed
sudo apt-get install -f

# Verify installation
dpkg -l | grep fix-fasttrade


**Package Information:**
* **Package Name**: fix-fasttrade
* **Section**: finance
* **Dependencies**: libboost-all-dev (>= 1.71)
* **Architecture**: amd64, arm64 (auto-detected)

RPM Packages (Red Hat/CentOS/Fedora)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Native RPM package format for Red Hat, CentOS, Fedora, and derivatives.

.. code-block:: bash

# Create RPM package
cpack -G RPM

# Output: FIX-FastTrade-2.0.0-Linux.rpm


**Installation:**
.. code-block:: bash

# Install the package (CentOS/RHEL)
sudo yum install FIX-FastTrade-2.0.0-Linux.rpm

# Install the package (Fedora)
sudo dnf install FIX-FastTrade-2.0.0-Linux.rpm

# Verify installation
rpm -qa | grep fix-fasttrade


**Package Information:**
* **Package Name**: FIX-FastTrade
* **Group**: Applications/Finance
* **Requires**: boost >= 1.71
* **Architecture**: x86_64, aarch64 (auto-detected)

macOS Package Formats
~~~~~~~~~~~~~~~~~~~~~

macOS Bundle (Future)
^^^^^^^^^^^^^^^^^^^^^
Native macOS application bundle (planned for future releases).

.. code-block:: bash

# Create macOS bundle (when implemented)
cpack -G Bundle

# Output: FIX-FastTrade-2.0.0-Darwin.dmg


Windows Package Formats
~~~~~~~~~~~~~~~~~~~~~~~

NSIS Installer (Future)
^^^^^^^^^^^^^^^^^^^^^^^
Windows installer using NSIS (planned for future releases).

.. code-block:: bash

# Create Windows installer (when implemented)
cpack -G NSIS

# Output: FIX-FastTrade-2.0.0-win64.exe


Package Configuration
---------------------

Basic Package Information
~~~~~~~~~~~~~~~~~~~~~~~~~

The following information is included in all packages:

.. code-block:: cmake

set(CPACK_PACKAGE_NAME "FIX-FastTrade")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "Ultra-low latency FIX protocol trading system with Phase 1 optimizations")
set(CPACK_PACKAGE_CONTACT "FIX-FastTrade Team")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")


Platform-Specific Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Debian Package Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cmake

# Dependencies
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-all-dev (>= 1.71)")

# Package classification
set(CPACK_DEBIAN_PACKAGE_SECTION "finance")

# Maintainer information
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "FIX-FastTrade Team")

# Package priority
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

# Architecture (auto-detected)
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")


RPM Package Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cmake

# Dependencies
set(CPACK_RPM_PACKAGE_REQUIRES "boost >= 1.71")

# Package classification
set(CPACK_RPM_PACKAGE_GROUP "Applications/Finance")

# License information
set(CPACK_RPM_PACKAGE_LICENSE "MIT")

# Vendor information
set(CPACK_RPM_PACKAGE_VENDOR "FIX-FastTrade Project")

# Architecture (auto-detected)
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")


Package Contents
----------------

Standard Package Contents
~~~~~~~~~~~~~~~~~~~~~~~~~

All packages include the following components:

Executables
^^^^^^^^^^^
* ``bin/fix-fasttrade`` - Main trading system executable

Libraries
^^^^^^^^^
* ``lib/libfix-fasttrade-core.a`` - Static library for integration

Header Files
^^^^^^^^^^^^
* ``include/fix/`` - FIX protocol headers
* ``include/model/`` - Trading model headers
* ``include/service/`` - Service layer headers
* ``include/infrastructure/`` - Infrastructure headers

Configuration Files
^^^^^^^^^^^^^^^^^^^
* ``config/fix-config.xml`` - Main configuration file
* ``config/examples/`` - Example configurations
* ``config/schemas/`` - XML schema files

Documentation
^^^^^^^^^^^^^
* ``docs/`` - API documentation (if generated)
* ``README.md`` - Project documentation
* ``LICENSE`` - License file

Package File Layout
~~~~~~~~~~~~~~~~~~~


/usr/local/                    # Default installation prefix
├── bin/
│   └── fix-fasttrade         # Main executable
├── lib/
│   └── libfix-fasttrade-core.a  # Static library
├── include/
│   ├── fix/                  # FIX protocol headers
│   ├── model/                # Trading model headers
│   ├── service/              # Service layer headers
│   └── infrastructure/       # Infrastructure headers
├── config/
│   ├── fix-config.xml        # Main configuration
│   ├── examples/             # Example configurations
│   └── schemas/              # XML schemas
└── docs/                     # Documentation (optional)
    └── html/                 # Generated API docs


Creating Packages
-----------------

Basic Package Creation
~~~~~~~~~~~~~~~~~~~~~~

Create All Package Types
^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Build the project first
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Create all configured package types
cpack

# Output files:
# - FIX-FastTrade-2.0.0-Linux.zip
# - FIX-FastTrade-2.0.0-Linux.tar.gz
# - FIX-FastTrade-2.0.0-Linux.deb
# - FIX-FastTrade-2.0.0-Linux.rpm


Create Specific Package Types
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Create only DEB package
cpack -G DEB

# Create only RPM package
cpack -G RPM

# Create only ZIP archive
cpack -G ZIP

# Create multiple specific types
cpack -G "DEB;RPM"


Advanced Package Creation
~~~~~~~~~~~~~~~~~~~~~~~~~

Custom Package Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Override package name
cpack -D CPACK_PACKAGE_NAME="CustomFIXTrade"

# Override version
cpack -D CPACK_PACKAGE_VERSION="2.1.0"

# Override description
cpack -D CPACK_PACKAGE_DESCRIPTION="Custom FIX Trading System"

# Combine multiple overrides
cpack -D CPACK_PACKAGE_NAME="CustomFIXTrade" \
      -D CPACK_PACKAGE_VERSION="2.1.0" \
      -G DEB


Package Output Directory
^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Create packages in specific directory
cpack -B /tmp/packages

# Create packages with custom config
cpack --config CPackConfig.cmake -B /tmp/packages


Verbose Package Creation
^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Enable verbose output
cpack --verbose

# Enable debug output
cpack --debug


Platform-Specific Package Creation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Linux Package Creation
^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Ubuntu/Debian systems
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cpack -G DEB

# CentOS/RHEL/Fedora systems
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cpack -G RPM

# Generic Linux (portable)
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-mtune=generic"
make -j$(nproc)
cpack -G TGZ


macOS Package Creation
^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# macOS with Homebrew dependencies
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0
make -j$(sysctl -n hw.ncpu)
cpack -G TGZ

# Universal binary (Intel + Apple Silicon)
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
         -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0
make -j$(sysctl -n hw.ncpu)
cpack -G TGZ


Windows Package Creation
^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: cmd

REM Windows with Visual Studio
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel
cpack -G ZIP


Package Installation
--------------------

Linux Package Installation
~~~~~~~~~~~~~~~~~~~~~~~~~~

DEB Package Installation
^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Install package
sudo dpkg -i FIX-FastTrade-2.0.0-Linux.deb

# Fix dependencies if needed
sudo apt-get install -f

# Verify installation
dpkg -l | grep fix-fasttrade

# Check installed files
dpkg -L fix-fasttrade

# Remove package
sudo dpkg -r fix-fasttrade


RPM Package Installation
^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Install package (CentOS/RHEL)
sudo yum localinstall FIX-FastTrade-2.0.0-Linux.rpm

# Install package (Fedora)
sudo dnf install FIX-FastTrade-2.0.0-Linux.rpm

# Verify installation
rpm -qa | grep FIX-FastTrade

# Check installed files
rpm -ql FIX-FastTrade

# Remove package
sudo yum remove FIX-FastTrade  # CentOS/RHEL
sudo dnf remove FIX-FastTrade  # Fedora


Archive Installation
~~~~~~~~~~~~~~~~~~~~

ZIP/TGZ Archive Installation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Extract archive
unzip FIX-FastTrade-2.0.0-Linux.zip
# or
tar -xzf FIX-FastTrade-2.0.0-Linux.tar.gz

# Install to system location
sudo cp -r FIX-FastTrade-2.0.0-Linux/* /usr/local/

# Install to user location
cp -r FIX-FastTrade-2.0.0-Linux/* ~/.local/

# Update PATH if needed
echo 'export PATH=$HOME/.local/bin:$PATH' >> ~/.bashrc
source ~/.bashrc


Package Verification
--------------------

Package Integrity
~~~~~~~~~~~~~~~~~
.. code-block:: bash

# Verify DEB package
dpkg --info FIX-FastTrade-2.0.0-Linux.deb
dpkg --contents FIX-FastTrade-2.0.0-Linux.deb

# Verify RPM package
rpm -qip FIX-FastTrade-2.0.0-Linux.rpm
rpm -qlp FIX-FastTrade-2.0.0-Linux.rpm

# Verify archive contents
unzip -l FIX-FastTrade-2.0.0-Linux.zip
tar -tzf FIX-FastTrade-2.0.0-Linux.tar.gz


Installation Verification
~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: bash

# Check executable
which fix-fasttrade
fix-fasttrade --version
fix-fasttrade --help

# Check library
ls -la /usr/local/lib/libfix-fasttrade-core.a

# Check headers
ls -la /usr/local/include/fix/

# Check configuration
ls -la /usr/local/config/


Custom Package Configuration
----------------------------

Creating Custom Package Types
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Custom DEB Configuration
^^^^^^^^^^^^^^^^^^^^^^^^
Create a custom CPack configuration file:

.. code-block:: cmake

# CustomDEB.cmake
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "custom-fix-fasttrade")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-all-dev (>= 1.71), libssl-dev")
set(CPACK_DEBIAN_PACKAGE_SECTION "finance")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Custom Maintainer <maintainer@example.com>")

# Use custom configuration
cpack --config CustomDEB.cmake


Custom RPM Configuration
^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: cmake

# CustomRPM.cmake
set(CPACK_GENERATOR "RPM")
set(CPACK_PACKAGE_NAME "custom-fix-fasttrade")
set(CPACK_RPM_PACKAGE_REQUIRES "boost >= 1.71, openssl-devel")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Finance")
set(CPACK_RPM_PACKAGE_VENDOR "Custom Vendor")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")

# Use custom configuration
cpack --config CustomRPM.cmake


Package Components
~~~~~~~~~~~~~~~~~~

Creating Component-Based Packages
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: cmake

# Define components in CMakeLists.txt
install(TARGETS fix-fasttrade
    RUNTIME DESTINATION bin
    COMPONENT Runtime
)

install(TARGETS fix-fasttrade-core
    ARCHIVE DESTINATION lib
    COMPONENT Development
)

install(DIRECTORY include/
    DESTINATION include
    COMPONENT Development
)

# Create component-specific packages
cpack -D CPACK_COMPONENTS_ALL="Runtime"
cpack -D CPACK_COMPONENTS_ALL="Development"
cpack -D CPACK_COMPONENTS_ALL="Runtime;Development"


Troubleshooting Package Creation
--------------------------------

Common Issues
~~~~~~~~~~~~~

Missing Dependencies
^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Error: Package dependencies not found
# Solution: Install development packages
sudo apt install libboost-all-dev  # Ubuntu/Debian
sudo yum install boost-devel        # CentOS/RHEL


Permission Issues
^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Error: Permission denied during package creation
# Solution: Ensure write permissions
chmod 755 build/
cpack -B /tmp/packages  # Use writable directory


Architecture Mismatch
^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Error: Wrong architecture in package
# Solution: Specify target architecture
cmake .. -DCMAKE_SYSTEM_PROCESSOR=x86_64
cpack -G DEB


Large Package Size
^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Issue: Package too large
# Solution: Strip debug symbols
cmake .. -DCMAKE_BUILD_TYPE=Release
strip build/bin/fix-fasttrade
cpack


Package Validation
~~~~~~~~~~~~~~~~~~

Pre-Installation Testing
^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Test package installation in container
docker run -it ubuntu:20.04
apt update && apt install -y ./FIX-FastTrade-2.0.0-Linux.deb
fix-fasttrade --help

# Test package removal
dpkg -r fix-fasttrade


Post-Installation Testing
^^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: bash

# Verify all files installed correctly
dpkg -L fix-fasttrade | xargs ls -la

# Test executable functionality
fix-fasttrade --version
fix-fasttrade --config /usr/local/config/fix-config.xml --help


This comprehensive packaging guide provides all the information needed to create, distribute, and install FIX-FastTrade packages across different platforms and package managers.