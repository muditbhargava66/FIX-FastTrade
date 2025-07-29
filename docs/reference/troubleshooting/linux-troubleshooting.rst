Linux Troubleshooting Guide
===========================

This guide covers common issues and solutions when building and running FIX-FastTrade on various Linux distributions, including Ubuntu, CentOS, RHEL, Fedora, Debian, and Arch Linux.

System Requirements
-------------------

Minimum Requirements
~~~~~~~~~~~~~~~~~~~~
* **Linux Kernel**: 4.15+ (Ubuntu 18.04+, CentOS 8+, Debian 10+)
* **GCC**: 8.0+ or Clang 7.0+
* **RAM**: 4GB minimum, 8GB recommended
* **Storage**: 2GB free space for build

Recommended Requirements
~~~~~~~~~~~~~~~~~~~~~~~~
* **Linux Kernel**: 5.4+ for optimal performance
* **GCC**: 9.0+ or Clang 10.0+
* **RAM**: 16GB for optimal build performance
* **CPU**: Modern x86_64 or ARM64 with SIMD support

Distribution-Specific Issues
----------------------------

Ubuntu/Debian Issues
~~~~~~~~~~~~~~~~~~~~

Issue 1: Package Repository Not Updated
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: bash

E: Unable to locate package libboost-all-dev
Package 'cmake' has no installation candidate


**Solution:**
.. code-block:: bash

# Update package lists
sudo apt update

# If repositories are outdated, upgrade
sudo apt upgrade

# For older Ubuntu versions, enable universe repository
sudo add-apt-repository universe
sudo apt update

# Verify package availability
apt search libboost-all-dev
apt policy cmake


Issue 2: CMake Version Too Old
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: bash

CMake 3.10 or higher is required. You are running version 3.5.1


**Solution for Ubuntu 18.04+:**
.. code-block:: bash

# Install newer CMake from official repository
sudo apt remove cmake
sudo apt install snapd
sudo snap install cmake --classic

# Verify version
cmake --version
# Should show 3.20+


**Solution for Older Ubuntu/Debian:**
.. code-block:: bash

# Install CMake from Kitware repository
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null

# Add repository
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'
sudo apt update

# Install latest CMake
sudo apt install cmake


Issue 3: Boost Version Incompatibility
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: bash

error: 'boost::filesystem::path' has no member named 'generic_string'
Boost version 1.65 is too old


**Solution:**
.. code-block:: bash

# Check current Boost version
dpkg -l | grep libboost
pkg-config --modversion boost

# For Ubuntu 20.04+, install newer Boost
sudo apt install libboost-all-dev

# For older versions, build from source
wget https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.tar.gz
tar -xzf boost_1_82_0.tar.gz
cd boost_1_82_0
./bootstrap.sh --prefix=/usr/local/boost
./b2 -j$(nproc)
sudo ./b2 install

# Configure CMake to use custom Boost
cmake .. -DBOOST_ROOT=/usr/local/boost


CentOS/RHEL/Rocky Linux Issues
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Issue 1: Development Tools Not Installed
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: bash

gcc: command not found
make: command not found


**Solution for CentOS 8+/RHEL 8+:**
.. code-block:: bash

# Install development tools group
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake pkgconfig git

# Verify installation
gcc --version
make --version
cmake --version


**Solution for CentOS 7/RHEL 7:**
.. code-block:: bash

# Install development tools
sudo yum groupinstall "Development Tools"
sudo yum install cmake3 pkgconfig git

# Use cmake3 instead of cmake
alias cmake=cmake3
echo 'alias cmake=cmake3' >> ~/.bashrc


Issue 2: EPEL Repository Missing
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: bash

No package boost-devel available
Package cmake not found


**Solution:**
.. code-block:: bash

# Install EPEL repository
sudo dnf install epel-release  # CentOS 8+
# or
sudo yum install epel-release  # CentOS 7

# Update package cache
sudo dnf update  # or sudo yum update

# Install packages
sudo dnf install boost-devel cmake pkgconfig


Issue 3: GCC Version Too Old
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: bash

error: #error This file requires compiler and library support for the ISO C++ 2017 standard


**Solution for CentOS 7:**
.. code-block:: bash

# Install Software Collections (SCL)
sudo yum install centos-release-scl
sudo yum install devtoolset-9-gcc devtoolset-9-gcc-c++

# Enable newer GCC
scl enable devtoolset-9 bash
# or add to ~/.bashrc:
echo 'source /opt/rh/devtoolset-9/enable' >> ~/.bashrc

# Verify version
gcc --version
# Should show GCC 9.x


Fedora Issues
~~~~~~~~~~~~~

Issue 1: DNF Package Conflicts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: bash

Problem: conflicting requests
nothing provides boost-devel needed by package


**Solution:**
.. code-block:: bash

# Update system first
sudo dnf update

# Install development group
sudo dnf groupinstall "C Development Tools and Libraries"

# Install specific packages
sudo dnf install gcc-c++ cmake pkgconfig boost-devel git

# If conflicts persist, use --allowerasing
sudo dnf install --allowerasing boost-devel


Arch Linux Issues
~~~~~~~~~~~~~~~~~

Issue 1: AUR Package Build Failures
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: bash

error: failed to prepare transaction (could not satisfy dependencies)
makepkg: error: failed to build package


**Solution:**
.. code-block:: bash

# Update system first
sudo pacman -Syu

# Install base development tools
sudo pacman -S base-devel cmake pkgconfig boost git

# For AUR packages, use yay or paru
yay -S quickfix  # if available in AUR

# Or build manually
git clone https://aur.archlinux.org/quickfix.git
cd quickfix
makepkg -si


Common Build Issues
-------------------

Issue 1: Compiler Not Found
~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

CMAKE_CXX_COMPILER not set, after EnableLanguage
No CMAKE_C_COMPILER could be found


**Solution:**
.. code-block:: bash

# Install compiler
sudo apt install build-essential  # Ubuntu/Debian
sudo dnf install gcc-c++          # Fedora/CentOS 8+
sudo yum install gcc-c++          # CentOS 7
sudo pacman -S gcc                # Arch

# Specify compiler explicitly
cmake .. \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_C_COMPILER=gcc

# Or use Clang
sudo apt install clang  # Ubuntu/Debian
cmake .. \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_C_COMPILER=clang


Issue 2: Missing pkg-config
~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

Could NOT find PkgConfig (missing: PKG_CONFIG_EXECUTABLE)


**Solution:**
.. code-block:: bash

# Install pkg-config
sudo apt install pkg-config        # Ubuntu/Debian
sudo dnf install pkgconfig         # Fedora/CentOS 8+
sudo yum install pkgconfig         # CentOS 7
sudo pacman -S pkgconfig          # Arch

# Verify installation
pkg-config --version
which pkg-config


Issue 3: QuickFIX Build Failures
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

fatal error: quickfix/Application.h: No such file or directory
Could not find QuickFIX library


**Solution:**
.. code-block:: bash

# Try system package first
sudo apt install libquickfix-dev   # Ubuntu/Debian
sudo dnf install quickfix-devel    # Fedora (if available)

# If not available, use git submodule
git submodule update --init --recursive

# Verify submodule
ls -la third_party/quickfix/

# Build will automatically compile QuickFIX from source


Issue 4: Memory Issues During Build
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

c++: internal compiler error: Killed (program cc1plus)
virtual memory exhausted: Cannot allocate memory


**Solution:**
.. code-block:: bash

# Check available memory
free -h

# Reduce parallel jobs
make -j2  # Instead of make -j$(nproc)

# Add swap space temporarily
sudo fallocate -l 2G /tmp/swapfile
sudo chmod 600 /tmp/swapfile
sudo mkswap /tmp/swapfile
sudo swapon /tmp/swapfile

# Build with reduced parallelism
make -j1

# Remove swap after build
sudo swapoff /tmp/swapfile
sudo rm /tmp/swapfile


Issue 5: Linker Errors
~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

/usr/bin/ld: cannot find -lboost_system
undefined reference to `boost::system::error_category::name()'


**Solution:**
.. code-block:: bash

# Install development packages
sudo apt install libboost-all-dev  # Ubuntu/Debian
sudo dnf install boost-devel       # Fedora/CentOS

# Check library installation
ldconfig -p | grep boost
pkg-config --libs boost

# If libraries are in non-standard location
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH

# Or specify library path in CMake
cmake .. -DCMAKE_LIBRARY_PATH=/usr/local/lib


Runtime Issues
--------------

Issue 1: Shared Library Not Found
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

./fix-fasttrade: error while loading shared libraries: libboost_system.so.1.71.0: cannot open shared object file


**Solution:**
.. code-block:: bash

# Check library dependencies
ldd ./bin/fix-fasttrade

# Find missing library
find /usr -name "libboost_system.so*" 2>/dev/null

# Add library path
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# Or create symlink
sudo ln -s /usr/lib/x86_64-linux-gnu/libboost_system.so.1.74.0 /usr/lib/libboost_system.so.1.71.0

# Permanent solution: update ldconfig
echo "/usr/local/lib" | sudo tee /etc/ld.so.conf.d/local.conf
sudo ldconfig


Issue 2: Permission Denied
~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

bind: Permission denied
Cannot access configuration file


**Solution:**
.. code-block:: bash

# For privileged ports (< 1024)
sudo ./bin/fix-fasttrade --config config/fix-config.xml

# Or change to unprivileged port in config
sed -i 's/port="443"/port="9876"/' config/fix-config.xml

# Fix file permissions
chmod 644 config/*.xml
chmod 755 bin/fix-fasttrade

# Check SELinux (if enabled)
getenforce
# If enforcing, temporarily disable or configure
sudo setenforce 0  # Temporary
# Or configure SELinux policy for your application


Issue 3: High CPU Usage
~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: bash

CPU usage at 100%
System becomes unresponsive


**Solution:**
.. code-block:: bash

# Monitor CPU usage
top -p $(pgrep fix-fasttrade)
htop -p $(pgrep fix-fasttrade)

# Check CPU frequency scaling
cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# Set performance governor
echo 'performance' | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# Limit CPU affinity
taskset -c 0,1 ./bin/fix-fasttrade --config config/fix-config.xml

# Or use cgroups for resource limiting
sudo cgcreate -g cpu:/fix-fasttrade
echo 50000 | sudo tee /sys/fs/cgroup/cpu/fix-fasttrade/cpu.cfs_quota_us
sudo cgexec -g cpu:fix-fasttrade ./bin/fix-fasttrade --config config/fix-config.xml


Performance Optimization
------------------------

CPU Optimization
~~~~~~~~~~~~~~~~

.. code-block:: bash

# Disable CPU frequency scaling
echo 'performance' | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# Disable NUMA balancing
echo 0 | sudo tee /proc/sys/kernel/numa_balancing

# Set CPU affinity
taskset -c 0-3 ./bin/fix-fasttrade --config config/fix-config.xml

# Or use numactl for NUMA systems
numactl --cpunodebind=0 --membind=0 ./bin/fix-fasttrade --config config/fix-config.xml


Memory Optimization
~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

# Allow memory locking
ulimit -l unlimited

# Disable swap
sudo swapoff -a

# Configure huge pages
echo 1024 | sudo tee /proc/sys/vm/nr_hugepages
echo always | sudo tee /sys/kernel/mm/transparent_hugepage/enabled

# Run with memory optimizations
./bin/fix-fasttrade \
    --config config/fix-config.xml \
    --memory-lock \
    --huge-pages


Network Optimization
~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

# Increase network buffer sizes
echo 'net.core.rmem_max = 134217728' | sudo tee -a /etc/sysctl.conf
echo 'net.core.wmem_max = 134217728' | sudo tee -a /etc/sysctl.conf
echo 'net.ipv4.tcp_rmem = 4096 87380 134217728' | sudo tee -a /etc/sysctl.conf
echo 'net.ipv4.tcp_wmem = 4096 65536 134217728' | sudo tee -a /etc/sysctl.conf

# Apply settings
sudo sysctl -p

# Monitor network performance
ss -i  # Show socket information
iftop  # Monitor network traffic


SIMD and Architecture-Specific Issues
-------------------------------------

x86_64 SIMD Issues
~~~~~~~~~~~~~~~~~~

Issue: Illegal Instruction Error
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: bash

Illegal instruction (core dumped)
SIGILL signal received


**Solution:**
.. code-block:: bash

# Check CPU capabilities
cat /proc/cpuinfo | grep flags
lscpu | grep -i flags

# Build with conservative SIMD flags
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_SIMD=ON \
    -DCMAKE_CXX_FLAGS="-msse4.2"  # Conservative

# Or disable SIMD entirely
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_SIMD=OFF

# Test SIMD support
./bin/fix-fasttrade --test-simd


ARM64 SIMD Issues
~~~~~~~~~~~~~~~~~

Issue: NEON Instructions Not Available
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Symptoms:**
.. code-block:: bash

Unsupported instruction set
NEON not available on this CPU


**Solution:**
.. code-block:: bash

# Check ARM CPU features
cat /proc/cpuinfo | grep Features
lscpu | grep -i flags

# Build with NEON support
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_SIMD=ON \
    -DCMAKE_CXX_FLAGS="-march=native -mtune=native"

# For specific ARM processors
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_SIMD=ON \
    -DCMAKE_CXX_FLAGS="-mcpu=cortex-a72 -mfpu=neon"


Debugging Tools
---------------

Using GDB
~~~~~~~~~

.. code-block:: bash

# Install GDB
sudo apt install gdb  # Ubuntu/Debian
sudo dnf install gdb  # Fedora/CentOS

# Debug with GDB
gdb ./bin/fix-fasttrade
(gdb) set args --config config/fix-config.xml
(gdb) run
(gdb) bt  # backtrace on crash
(gdb) info registers
(gdb) disassemble  # show assembly


Using Valgrind
~~~~~~~~~~~~~~

.. code-block:: bash

# Install Valgrind
sudo apt install valgrind  # Ubuntu/Debian
sudo dnf install valgrind  # Fedora/CentOS

# Memory leak detection
valgrind --leak-check=full --show-leak-kinds=all ./bin/fix-fasttrade --config config/fix-config.xml

# Performance profiling
valgrind --tool=callgrind ./bin/fix-fasttrade --config config/fix-config.xml
kcachegrind callgrind.out.*


Using perf
~~~~~~~~~~

.. code-block:: bash

# Install perf
sudo apt install linux-tools-generic  # Ubuntu
sudo dnf install perf                  # Fedora/CentOS

# Profile CPU usage
sudo perf record -g ./bin/fix-fasttrade --config config/fix-config.xml
sudo perf report

# Monitor system performance
sudo perf top -p $(pgrep fix-fasttrade)

# Analyze cache misses
sudo perf stat -e cache-misses,cache-references ./bin/fix-fasttrade --config config/fix-config.xml


System Monitoring
-----------------

Resource Monitoring
~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

# CPU and memory usage
htop
top -p $(pgrep fix-fasttrade)

# I/O monitoring
iotop
iostat -x 1

# Network monitoring
iftop
netstat -i
ss -tuln

# System calls
strace -p $(pgrep fix-fasttrade)


Log Analysis
~~~~~~~~~~~~

.. code-block:: bash

# System logs
journalctl -u fix-fasttrade
tail -f /var/log/syslog | grep fix-fasttrade

# Application logs
./bin/fix-fasttrade --config config/fix-config.xml --log-level debug 2>&1 | tee debug.log

# Core dump analysis
ulimit -c unlimited  # Enable core dumps
gdb ./bin/fix-fasttrade core


Getting Help
------------

Information Collection
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

# System information
uname -a
lsb_release -a  # Ubuntu/Debian
cat /etc/redhat-release  # CentOS/RHEL
cat /etc/os-release

# Hardware information
lscpu
cat /proc/meminfo
cat /proc/cpuinfo

# Compiler and library versions
gcc --version
clang --version
cmake --version
pkg-config --version
ldconfig -p | grep boost


Build Information
~~~~~~~~~~~~~~~~~

.. code-block:: bash

# CMake configuration
cmake .. -DCMAKE_BUILD_TYPE=Debug 2>&1 | tee cmake_config.log

# Build with verbose output
make VERBOSE=1 2>&1 | tee build.log

# Library dependencies
ldd ./bin/fix-fasttrade
objdump -p ./bin/fix-fasttrade | grep NEEDED


This comprehensive Linux troubleshooting guide should help resolve most issues encountered across different Linux distributions when building and running FIX-FastTrade.