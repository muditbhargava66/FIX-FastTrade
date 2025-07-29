# Installation Targets Guide

This document describes the CMake installation targets for FIX-FastTrade, including installation components, directory structure, and configuration options.

Overview
--------

FIX-FastTrade uses CMake's install system to provide a standardized installation process across different platforms. The installation system is designed to follow platform conventions while providing flexibility for custom deployments.

Installation Components
-----------------------

Runtime Component
~~~~~~~~~~~~~~~~~
Essential files needed to run FIX-FastTrade in production.

**Includes:**
* ``fix-fasttrade`` executable
* Runtime configuration files
* Essential documentation

**Installation:**
.. code-block:: bash

# Install only runtime components
cmake --build . --target install --component Runtime

# Or with make
make install/Runtime


Development Component
~~~~~~~~~~~~~~~~~~~~~
Files needed for development and integration with other projects.

**Includes:**
* ``libfix-fasttrade-core.a`` static library
* All header files
* CMake configuration files
* Development documentation

**Installation:**
.. code-block:: bash

# Install only development components
cmake --build . --target install --component Development

# Or with make
make install/Development


Documentation Component
~~~~~~~~~~~~~~~~~~~~~~~
Generated API documentation and guides.

**Includes:**
* Doxygen-generated HTML documentation
* API reference
* Design documentation

**Installation:**
.. code-block:: bash

# Install only documentation
cmake --build . --target install --component Documentation

# Or with make
make install/Documentation


Installation Targets
--------------------

Standard Installation
~~~~~~~~~~~~~~~~~~~~~
Install all components to the configured prefix.

.. code-block:: bash

# Install everything to default location (/usr/local)
make install

# Or with CMake directly
cmake --build . --target install


Custom Installation Prefix
~~~~~~~~~~~~~~~~~~~~~~~~~~
Install to a custom directory.

.. code-block:: bash

# Configure with custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/fix-fasttrade
make install

# Or reconfigure existing build
cmake . -DCMAKE_INSTALL_PREFIX=$HOME/fix-fasttrade
make install


User Installation
~~~~~~~~~~~~~~~~~
Install to user directory without root privileges.

.. code-block:: bash

# Install to user's local directory
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
make install

# Update PATH for user installation
echo 'export PATH=$HOME/.local/bin:$PATH' >> ~/.bashrc
source ~/.bashrc


System-Wide Installation
~~~~~~~~~~~~~~~~~~~~~~~~
Install to system directories (requires root).

.. code-block:: bash

# Install to system directories
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
sudo make install

# Alternative system locations
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local  # Default
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/fix-fasttrade  # Optional software


Installation Directory Structure
--------------------------------

Default Installation Layout
~~~~~~~~~~~~~~~~~~~~~~~~~~~
When installed with default prefix (``/usr/local``):


/usr/local/
├── bin/
│   └── fix-fasttrade                    # Main executable
├── lib/
│   └── libfix-fasttrade-core.a         # Static library
├── include/
│   ├── fix/                            # FIX protocol headers
│   │   ├── Application.h
│   │   ├── Config.h
│   │   ├── Message.h
│   │   ├── MessageParser.h
│   │   ├── Session.h
│   │   ├── SessionID.h
│   │   └── ZeroCopyMessage.h
│   ├── infrastructure/                  # Infrastructure headers
│   │   ├── containers/
│   │   │   └── SimpleQueue.h
│   │   ├── memory/
│   │   │   └── SimplePool.h
│   │   ├── platform/
│   │   │   └── Platform.h
│   │   ├── simd/
│   │   │   └── SIMDUtils.h
│   │   └── threading/
│   │       └── CPUAffinity.h
│   ├── model/                          # Trading model headers
│   │   ├── Order.h
│   │   └── Trade.h
│   └── service/                        # Service layer headers
│       ├── FixService.h
│       └── TradingService.h
├── config/                             # Configuration files
│   ├── fix-config.xml                  # Main configuration
│   ├── examples/                       # Example configurations
│   │   ├── basic-config.xml
│   │   ├── advanced-config.xml
│   │   └── test-config.xml
│   └── schemas/                        # XML schema files
│       └── fix-config.xsd
└── docs/                               # Documentation (optional)
    └── html/                           # Generated API docs
        ├── index.html
        ├── classes.html
        └── ...


Custom Installation Layout
~~~~~~~~~~~~~~~~~~~~~~~~~~
Example with custom prefix (``/opt/fix-fasttrade``):


/opt/fix-fasttrade/
├── bin/fix-fasttrade
├── lib/libfix-fasttrade-core.a
├── include/[same structure as above]
├── config/[same structure as above]
└── docs/[same structure as above]


User Installation Layout
~~~~~~~~~~~~~~~~~~~~~~~~
Example with user prefix (``$HOME/.local``):


$HOME/.local/
├── bin/fix-fasttrade
├── lib/libfix-fasttrade-core.a
├── include/[same structure as above]
├── config/[same structure as above]
└── docs/[same structure as above]


Installation Configuration
--------------------------

CMake Installation Variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``CMAKE_INSTALL_PREFIX``
^^^^^^^^^^^^^^^^^^^^^^^^
Base directory for installation.

.. code-block:: bash

# Default: /usr/local on Unix, C:/Program Files on Windows
cmake .. -DCMAKE_INSTALL_PREFIX=/custom/path


Component-Specific Directories
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Override specific installation directories:

.. code-block:: bash

# Custom binary directory
cmake .. -DCMAKE_INSTALL_BINDIR=bin

# Custom library directory
cmake .. -DCMAKE_INSTALL_LIBDIR=lib64

# Custom include directory
cmake .. -DCMAKE_INSTALL_INCLUDEDIR=include

# Custom configuration directory
cmake .. -DCMAKE_INSTALL_SYSCONFDIR=etc

# Custom documentation directory
cmake .. -DCMAKE_INSTALL_DOCDIR=share/doc


Full Path Overrides
^^^^^^^^^^^^^^^^^^^
Specify absolute paths for installation directories:

.. code-block:: bash

# Absolute paths
cmake .. \
    -DCMAKE_INSTALL_FULL_BINDIR=/usr/bin \
    -DCMAKE_INSTALL_FULL_LIBDIR=/usr/lib \
    -DCMAKE_INSTALL_FULL_INCLUDEDIR=/usr/include \
    -DCMAKE_INSTALL_FULL_SYSCONFDIR=/etc \
    -DCMAKE_INSTALL_FULL_DOCDIR=/usr/share/doc


Platform-Specific Installation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Linux Installation
^^^^^^^^^^^^^^^^^^
Follow Filesystem Hierarchy Standard (FHS):

.. code-block:: bash

# System installation (FHS compliant)
cmake .. \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DCMAKE_INSTALL_SYSCONFDIR=/etc \
    -DCMAKE_INSTALL_LOCALSTATEDIR=/var

sudo make install


macOS Installation
^^^^^^^^^^^^^^^^^^
Follow macOS conventions:

.. code-block:: bash

# macOS system installation
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install

# macOS application bundle (future)
cmake .. -DCMAKE_INSTALL_PREFIX=/Applications/FIX-FastTrade.app/Contents
make install


Windows Installation
^^^^^^^^^^^^^^^^^^^^
Follow Windows conventions:

.. code-block:: cmd

REM Windows system installation
cmake .. -DCMAKE_INSTALL_PREFIX="C:\Program Files\FIX-FastTrade"
cmake --build . --target install

REM Windows user installation
cmake .. -DCMAKE_INSTALL_PREFIX="%USERPROFILE%\AppData\Local\FIX-FastTrade"
cmake --build . --target install


Installation Examples
---------------------

Development Environment Setup
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Install for development with all components:

.. code-block:: bash

# Development installation
mkdir -p build-dev && cd build-dev
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$HOME/fix-fasttrade-dev
make -j$(nproc)
make install

# Verify development installation
ls -la $HOME/fix-fasttrade-dev/
$HOME/fix-fasttrade-dev/bin/fix-fasttrade --help


Production Deployment
~~~~~~~~~~~~~~~~~~~~~
Install optimized build for production:

.. code-block:: bash

# Production installation
mkdir -p build-prod && cd build-prod
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/opt/fix-fasttrade
make -j$(nproc)
sudo make install

# Create systemd service (Linux)
sudo tee /etc/systemd/system/fix-fasttrade.service << EOF
[Unit]
Description=FIX-FastTrade Trading System
After=network.target

[Service]
Type=simple
User=trading
ExecStart=/opt/fix-fasttrade/bin/fix-fasttrade --config /opt/fix-fasttrade/config/fix-config.xml
Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target
EOF

sudo systemctl enable fix-fasttrade
sudo systemctl start fix-fasttrade


Container Deployment
~~~~~~~~~~~~~~~~~~~~
Install for containerized deployment:

.. code-block:: bash

# Multi-stage Docker build
FROM ubuntu:20.04 AS builder
RUN apt-get update && apt-get install -y build-essential cmake libboost-all-dev
COPY . /src
WORKDIR /src/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/app
RUN make -j$(nproc) && make install

FROM ubuntu:20.04 AS runtime
RUN apt-get update && apt-get install -y libboost-system1.71.0 libboost-thread1.71.0
COPY --from=builder /app /app
EXPOSE 8080
CMD ["/app/bin/fix-fasttrade", "--config", "/app/config/fix-config.xml"]


Cross-Platform Installation
~~~~~~~~~~~~~~~~~~~~~~~~~~~
Install with portable configuration:

.. code-block:: bash

# Portable installation
mkdir -p build-portable && cd build-portable
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O2 -mtune=generic" \
    -DCMAKE_INSTALL_PREFIX=./install
make -j$(nproc)
make install

# Create portable package
tar -czf fix-fasttrade-portable.tar.gz -C install .


Post-Installation Configuration
-------------------------------

Environment Setup
~~~~~~~~~~~~~~~~~
Configure environment after installation:

.. code-block:: bash

# Add to PATH (if not in standard location)
export PATH=/opt/fix-fasttrade/bin:$PATH

# Add library path (if needed)
export LD_LIBRARY_PATH=/opt/fix-fasttrade/lib:$LD_LIBRARY_PATH

# Set configuration path
export FIX_FASTTRADE_CONFIG=/opt/fix-fasttrade/config/fix-config.xml

# Make permanent
echo 'export PATH=/opt/fix-fasttrade/bin:$PATH' >> ~/.bashrc


Configuration File Setup
~~~~~~~~~~~~~~~~~~~~~~~~
Set up configuration files after installation:

.. code-block:: bash

# Copy example configuration
cp /opt/fix-fasttrade/config/examples/basic-config.xml \
   /opt/fix-fasttrade/config/fix-config.xml

# Edit configuration
vim /opt/fix-fasttrade/config/fix-config.xml

# Validate configuration
/opt/fix-fasttrade/bin/fix-fasttrade --config /opt/fix-fasttrade/config/fix-config.xml --validate


Service Integration
~~~~~~~~~~~~~~~~~~~
Integrate with system services:

.. code-block:: bash

# Create systemd service (Linux)
sudo tee /etc/systemd/system/fix-fasttrade.service << EOF
[Unit]
Description=FIX-FastTrade Trading System
After=network.target

[Service]
Type=simple
User=trading
Group=trading
ExecStart=/opt/fix-fasttrade/bin/fix-fasttrade --config /opt/fix-fasttrade/config/fix-config.xml
WorkingDirectory=/opt/fix-fasttrade
Restart=always
RestartSec=5
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
EOF

# Enable and start service
sudo systemctl daemon-reload
sudo systemctl enable fix-fasttrade
sudo systemctl start fix-fasttrade
sudo systemctl status fix-fasttrade


Installation Verification
-------------------------

Basic Verification
~~~~~~~~~~~~~~~~~~
Verify installation completeness:

.. code-block:: bash

# Check executable
which fix-fasttrade
fix-fasttrade --version
fix-fasttrade --help

# Check library
ls -la /opt/fix-fasttrade/lib/libfix-fasttrade-core.a
nm /opt/fix-fasttrade/lib/libfix-fasttrade-core.a | grep -i session

# Check headers
ls -la /opt/fix-fasttrade/include/
find /opt/fix-fasttrade/include/ -name "*.h" | wc -l

# Check configuration
ls -la /opt/fix-fasttrade/config/
xmllint --noout /opt/fix-fasttrade/config/fix-config.xml


Functional Verification
~~~~~~~~~~~~~~~~~~~~~~~
Test installed system functionality:

.. code-block:: bash

# Test configuration validation
fix-fasttrade --config /opt/fix-fasttrade/config/examples/basic-config.xml --validate

# Test help system
fix-fasttrade --help

# Test version information
fix-fasttrade --version

# Test with minimal configuration
fix-fasttrade --config /opt/fix-fasttrade/config/examples/test-config.xml --dry-run


Integration Verification
~~~~~~~~~~~~~~~~~~~~~~~~
Test integration capabilities:

.. code-block:: bash

# Test library linking
cat > test_integration.cpp << EOF
#include <fix/Session.h>
#include <iostream>

int main() {
    std::cout << "FIX-FastTrade library integration test" << std::endl;
    return 0;
}
EOF

# Compile test
g++ -I/opt/fix-fasttrade/include test_integration.cpp \
    -L/opt/fix-fasttrade/lib -lfix-fasttrade-core \
    -lboost_system -lboost_thread -lpthread \
    -o test_integration

# Run test
./test_integration


Uninstallation
--------------

Manual Uninstallation
~~~~~~~~~~~~~~~~~~~~~
Remove installed files manually:

.. code-block:: bash

# Remove installed files
sudo rm -rf /opt/fix-fasttrade

# Remove service (if created)
sudo systemctl stop fix-fasttrade
sudo systemctl disable fix-fasttrade
sudo rm /etc/systemd/system/fix-fasttrade.service
sudo systemctl daemon-reload

# Remove user/group (if created)
sudo userdel trading
sudo groupdel trading


Package-Based Uninstallation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
If installed via package manager:

.. code-block:: bash

# Debian/Ubuntu
sudo dpkg -r fix-fasttrade

# CentOS/RHEL/Fedora
sudo yum remove FIX-FastTrade  # CentOS/RHEL
sudo dnf remove FIX-FastTrade  # Fedora


Installation Manifest
~~~~~~~~~~~~~~~~~~~~~
Create installation manifest for easier uninstallation:

.. code-block:: bash

# Create manifest during installation
make install > install_manifest.txt 2>&1

# Use manifest for uninstallation
cat install_manifest.txt | grep "Installing" | \
    sed 's/-- Installing: //' | \
    xargs sudo rm -f


This comprehensive installation targets guide provides all the information needed to properly install, configure, and manage FIX-FastTrade installations across different environments and use cases.