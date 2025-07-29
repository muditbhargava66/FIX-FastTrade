macOS Troubleshooting Guide
===========================

This guide covers common issues and solutions when building and running FIX-FastTrade on macOS systems, including both Apple Silicon (ARM64) and Intel (x86_64) architectures.

System Requirements
-------------------

Minimum Requirements
~~~~~~~~~~~~~~~~~~~~
* **macOS**: 10.15 (Catalina) for Intel, 11.0 (Big Sur) for Apple Silicon
* **Xcode Command Line Tools**: Latest version
* **RAM**: 4GB minimum, 8GB recommended
* **Storage**: 2GB free space for build

Recommended Requirements
~~~~~~~~~~~~~~~~~~~~~~~~
* **macOS**: 12.0 (Monterey) or later
* **RAM**: 16GB for optimal build performance
* **CPU**: Apple M1/M2 or Intel i7/i9 for best performance

Common Installation Issues
--------------------------

Issue 1: Xcode Command Line Tools Missing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

xcrun: error: invalid active developer path
clang: command not found


**Solution:**
.. code-block:: bash

# Install Xcode Command Line Tools
xcode-select --install

# If installation fails, reset and try again
sudo xcode-select --reset
xcode-select --install

# Verify installation
xcode-select -p
# Should output: /Applications/Xcode.app/Contents/Developer
# or: /Library/Developer/CommandLineTools


**Alternative Solution:**
.. code-block:: bash

# If automatic installation fails, download manually
# Go to: https://developer.apple.com/download/more/
# Search for "Command Line Tools for Xcode"
# Download and install the appropriate version


Issue 2: Homebrew Not Found or Misconfigured
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

brew: command not found
Error: No such file or directory @ rb_sysopen


**Solution for Apple Silicon (M1/M2):**
.. code-block:: bash

# Install Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Add to PATH (Apple Silicon)
echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zshrc
source ~/.zshrc

# Verify installation
brew --version
which brew
# Should output: /opt/homebrew/bin/brew


**Solution for Intel Macs:**
.. code-block:: bash

# Install Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Add to PATH (Intel)
echo 'eval "$(/usr/local/bin/brew shellenv)"' >> ~/.zshrc
source ~/.zshrc

# Verify installation
brew --version
which brew
# Should output: /usr/local/bin/brew


Issue 3: Boost Libraries Not Found
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

CMake Error: Could not find Boost
fatal error: 'boost/system.hpp' file not found


**Solution:**
.. code-block:: bash

# Install Boost via Homebrew
brew install boost

# Verify installation
brew list boost
pkg-config --cflags boost

# If still not found, specify path manually
cmake .. -DBOOST_ROOT=$(brew --prefix boost)

# For Apple Silicon, ensure correct architecture
cmake .. \
    -DBOOST_ROOT=$(brew --prefix boost) \
    -DCMAKE_OSX_ARCHITECTURES=arm64


**Alternative Solution (Manual Build):**
.. code-block:: bash

# Download and build Boost manually
wget https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.tar.gz
tar -xzf boost_1_82_0.tar.gz
cd boost_1_82_0

# For Apple Silicon
./bootstrap.sh --with-toolset=clang
./b2 architecture=arm64 --prefix=/usr/local/boost install

# For Intel
./bootstrap.sh --with-toolset=clang
./b2 architecture=x86 --prefix=/usr/local/boost install


Issue 4: QuickFIX Installation Problems
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

brew install quickfix
Error: No available formula with name "quickfix"


**Solution:**
.. code-block:: bash

# QuickFIX may not be available via Homebrew
# Use git submodule instead (recommended)
git submodule update --init --recursive

# Verify submodule is present
ls -la third_party/quickfix/

# Build will automatically compile QuickFIX from source


**Alternative Solution (Manual Build):**
.. code-block:: bash

# Clone and build QuickFIX manually
git clone https://github.com/quickfix/quickfix.git
cd quickfix
mkdir build && cd build

# Configure for macOS
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DHAVE_SSL=OFF \
    -DHAVE_MYSQL=OFF \
    -DHAVE_POSTGRESQL=OFF

# Build and install
make -j$(sysctl -n hw.ncpu)
sudo make install


Architecture-Specific Issues
----------------------------

Apple Silicon (ARM64) Issues
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Issue: Rosetta Translation Warnings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: bash

WARNING: Running under Rosetta translation
Performance may be degraded


**Solution:**
.. code-block:: bash

# Ensure native ARM64 build
cmake .. \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0

# Verify binary architecture
file ./bin/fix-fasttrade
# Should show: Mach-O 64-bit executable arm64

# Check if running under Rosetta
sysctl -n sysctl.proc_translated
# Should return: 0 (native) or 1 (Rosetta)


Issue: NEON SIMD Not Enabled
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: bash

SIMD optimizations not available
Performance lower than expected


**Solution:**
.. code-block:: bash

# Enable NEON SIMD for Apple Silicon
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_SIMD=ON \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -DCMAKE_CXX_FLAGS="-march=native -mtune=native"

# Verify SIMD support in build log
make VERBOSE=1 | grep -i neon


Issue: Homebrew Packages for Wrong Architecture
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: bash

dyld: Symbol not found: _some_symbol
Architecture mismatch errors


**Solution:**
.. code-block:: bash

# Check Homebrew architecture
brew config | grep -i cpu

# Reinstall packages for correct architecture
brew uninstall boost quickfix
brew install boost quickfix

# Force ARM64 installation
arch -arm64 brew install boost

# Verify package architecture
file $(brew --prefix boost)/lib/libboost_system.dylib
# Should show: Mach-O 64-bit dynamically linked shared library arm64


Intel (x86_64) Issues
~~~~~~~~~~~~~~~~~~~~~

Issue: AVX/SSE Instructions Not Available
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: bash

Illegal instruction (core dumped)
SIGILL signal received


**Solution:**
.. code-block:: bash

# Check CPU capabilities
sysctl -a | grep machdep.cpu.features
sysctl -a | grep machdep.cpu.leaf7_features

# Build with conservative flags
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_SIMD=ON \
    -DCMAKE_CXX_FLAGS="-msse4.2"  # Conservative SIMD

# Or disable SIMD entirely
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_SIMD=OFF


Build Issues
------------

Issue: CMake Configuration Fails
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

CMake Error: CMAKE_CXX_COMPILER not set
Could not find a package configuration file


**Solution:**
.. code-block:: bash

# Ensure Xcode tools are properly configured
sudo xcode-select --switch /Applications/Xcode.app/Contents/Developer
# or for Command Line Tools only:
sudo xcode-select --switch /Library/Developer/CommandLineTools

# Clear CMake cache and reconfigure
rm -rf build/
mkdir build && cd build

# Specify compiler explicitly
cmake .. \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_BUILD_TYPE=Release

# Verify compiler
clang++ --version


Issue: Linker Errors
~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

Undefined symbols for architecture arm64:
ld: symbol(s) not found for architecture arm64


**Solution:**
.. code-block:: bash

# Check library paths
otool -L ./bin/fix-fasttrade

# Add library search paths
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_LIBRARY_PATH="$(brew --prefix)/lib" \
    -DCMAKE_INCLUDE_PATH="$(brew --prefix)/include"

# For Apple Silicon, ensure consistent architecture
cmake .. \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -DCMAKE_FIND_ROOT_PATH=$(brew --prefix)


Issue: Compilation Errors with C++17
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

error: 'std::filesystem' is not available before macOS 10.15
error: unknown type name 'std::optional'


**Solution:**
.. code-block:: bash

# Set minimum deployment target
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 \
    -DCMAKE_CXX_STANDARD=17

# For Apple Silicon (requires macOS 11.0+)
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
    -DCMAKE_OSX_ARCHITECTURES=arm64


Runtime Issues
--------------

Issue: Dynamic Library Loading Errors
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

dyld: Library not loaded: /usr/local/lib/libboost_system.dylib
Reason: image not found


**Solution:**
.. code-block:: bash

# Check library dependencies
otool -L ./bin/fix-fasttrade

# Fix library paths using install_name_tool
install_name_tool -change \
    /usr/local/lib/libboost_system.dylib \
    $(brew --prefix)/lib/libboost_system.dylib \
    ./bin/fix-fasttrade

# Or set DYLD_LIBRARY_PATH (not recommended for production)
export DYLD_LIBRARY_PATH=$(brew --prefix)/lib:$DYLD_LIBRARY_PATH


Issue: Permission Denied Errors
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

Permission denied when accessing config files
Cannot bind to port: Permission denied


**Solution:**
.. code-block:: bash

# Fix file permissions
chmod 644 config/*.xml
chmod 755 bin/fix-fasttrade

# For privileged ports (< 1024), use sudo or change port
sudo ./bin/fix-fasttrade --config config/fix-config.xml

# Or modify config to use unprivileged port (>= 1024)
# Edit config/fix-config.xml and change port to 9876


Issue: High CPU Usage or Thermal Throttling
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

CPU usage consistently at 100%
System becomes unresponsive
Thermal throttling warnings


**Solution:**
.. code-block:: bash

# Monitor CPU usage
top -pid $(pgrep fix-fasttrade)

# Check thermal state
pmset -g thermstate

# Reduce CPU affinity to fewer cores
./bin/fix-fasttrade \
    --config config/fix-config.xml \
    --cpu-main 0 \
    --cpu-fix 1  # Use only 2 cores instead of all

# Enable thermal monitoring
sudo powermetrics -n 1 -i 5000 --samplers cpu_power,thermal


Performance Optimization
------------------------

Optimal Build Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

# High-performance build for Apple Silicon
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
    -DENABLE_SIMD=ON \
    -DENABLE_LTO=ON \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -mtune=native -flto"

# High-performance build for Intel
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES=x86_64 \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 \
    -DENABLE_SIMD=ON \
    -DENABLE_LTO=ON \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -mtune=native -mavx2 -flto"


Runtime Performance Tuning
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

# Optimal runtime configuration
./bin/fix-fasttrade \
    --config config/fix-config.xml \
    --cpu-main 0 \
    --cpu-fix 1 \
    --cpu-order 2 \
    --memory-lock \
    --priority -10 \
    --stats-interval 60

# Monitor performance
sudo dtrace -n 'pid$target:::entry { @[probefunc] = count(); }' -p $(pgrep fix-fasttrade)


Debugging Tools
---------------

Using Xcode Instruments
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

# Profile with Instruments
instruments -t "Time Profiler" ./bin/fix-fasttrade --config config/fix-config.xml

# Memory analysis
instruments -t "Allocations" ./bin/fix-fasttrade --config config/fix-config.xml

# System trace
instruments -t "System Trace" ./bin/fix-fasttrade --config config/fix-config.xml


Using LLDB Debugger
~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

# Debug with LLDB
lldb ./bin/fix-fasttrade
(lldb) settings set target.run-args --config config/fix-config.xml
(lldb) run
(lldb) bt  # backtrace on crash
(lldb) frame variable  # inspect variables


System Monitoring
~~~~~~~~~~~~~~~~~

.. code-block:: bash

# Monitor system resources
sudo powermetrics -n 0 -i 5000 --samplers cpu_power,gpu_power,thermal,network,disk

# Network monitoring
sudo tcpdump -i any port 9876

# File system monitoring
sudo fs_usage -w -f filesys fix-fasttrade


Getting Help
------------

Log Collection
~~~~~~~~~~~~~~

.. code-block:: bash

# Collect system information
system_profiler SPHardwareDataType SPSoftwareDataType > system_info.txt

# Collect build logs
cmake .. 2>&1 | tee cmake_config.log
make VERBOSE=1 2>&1 | tee build.log

# Collect runtime logs
./bin/fix-fasttrade --config config/fix-config.xml --log-level debug 2>&1 | tee runtime.log


Useful Commands for Support
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

# System information
uname -a
sw_vers
sysctl -n machdep.cpu.brand_string
sysctl -n hw.ncpu
sysctl -n hw.memsize

# Compiler information
clang++ --version
cmake --version
brew --version

# Library information
brew list --versions boost quickfix
pkg-config --modversion boost


This troubleshooting guide should help resolve most common issues encountered when building and running FIX-FastTrade on macOS systems.