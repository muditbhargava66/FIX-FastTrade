Windows Troubleshooting Guide
=============================

This guide covers common issues and solutions when building and running FIX-FastTrade on Windows systems, including Windows 10, Windows 11, and Windows Server.

System Requirements
-------------------

Minimum Requirements
~~~~~~~~~~~~~~~~~~~~
* **Windows**: 10 version 1909 or later, Windows 11, or Windows Server 2019+
* **Visual Studio**: 2019 or 2022 with C++ development tools
* **RAM**: 8GB minimum, 16GB recommended
* **Storage**: 5GB free space for build and dependencies

Recommended Requirements
~~~~~~~~~~~~~~~~~~~~~~~~
* **Windows**: 11 with latest updates
* **Visual Studio**: 2022 with latest updates
* **RAM**: 32GB for optimal build performance
* **CPU**: Intel i7/i9 or AMD Ryzen 7/9 for best performance

Common Installation Issues
--------------------------

Issue 1: Visual Studio Not Found
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: cmd

'cl' is not recognized as an internal or external command
MSVC compiler not found


**Solution:**
.. code-block:: cmd

REM Check if Visual Studio is installed
where cl

REM If not found, install Visual Studio with C++ tools
REM Download from: https://visualstudio.microsoft.com/downloads/

REM Or install Build Tools only:
REM https://visualstudio.microsoft.com/visual-cpp-build-tools/

REM After installation, open "Developer Command Prompt for VS"
REM or run vcvarsall.bat:
"C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM Verify installation
cl


**Alternative Solution (Visual Studio Installer):**
.. code-block:: cmd

REM Use Visual Studio Installer to modify existing installation
REM Add "C++ CMake tools for Visual Studio"
REM Add "MSVC v143 - VS 2022 C++ x64/x86 build tools"
REM Add "Windows 10/11 SDK (latest version)"


Issue 2: CMake Not Found
~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: cmd

'cmake' is not recognized as an internal or external command


**Solution:**
.. code-block:: cmd

REM Install CMake from official website
REM Download from: https://cmake.org/download/

REM Or use winget (Windows Package Manager)
winget install Kitware.CMake

REM Or use Chocolatey
choco install cmake

REM Add CMake to PATH if not automatically added
set PATH=%PATH%;C:\Program Files\CMake\bin

REM Verify installation
cmake --version


Issue 3: Git Not Available
~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: cmd

'git' is not recognized as an internal or external command


**Solution:**
.. code-block:: cmd

REM Install Git for Windows
REM Download from: https://git-scm.com/download/win

REM Or use winget
winget install Git.Git

REM Or use Chocolatey
choco install git

REM Verify installation
git --version


Issue 4: vcpkg Installation Issues
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: cmd

vcpkg is not recognized as an internal or external command
Error: Could not find vcpkg.exe


**Solution:**
.. code-block:: cmd

REM Clone vcpkg repository
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

REM Bootstrap vcpkg
.\bootstrap-vcpkg.bat

REM Integrate with Visual Studio (optional but recommended)
.\vcpkg integrate install

REM Add vcpkg to PATH
set PATH=%PATH%;C:\vcpkg

REM Verify installation
vcpkg version


Dependency Installation Issues
------------------------------

Issue 1: Boost Installation Fails
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: cmd

error: package boost:x64-windows failed to build
Could not find Boost libraries


**Solution:**
.. code-block:: cmd

REM Install Boost via vcpkg
.\vcpkg install boost:x64-windows

REM If installation fails, try individual components
.\vcpkg install boost-system:x64-windows
.\vcpkg install boost-thread:x64-windows
.\vcpkg install boost-filesystem:x64-windows
.\vcpkg install boost-date-time:x64-windows
.\vcpkg install boost-program-options:x64-windows
.\vcpkg install boost-log:x64-windows

REM Check installation
.\vcpkg list | findstr boost

REM If still failing, clean and retry
.\vcpkg remove boost:x64-windows
rmdir /s /q buildtrees\boost
.\vcpkg install boost:x64-windows


Issue 2: QuickFIX Not Available
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: cmd

error: package quickfix:x64-windows not found
Could not find QuickFIX library


**Solution:**
.. code-block:: cmd

REM QuickFIX may not be available in vcpkg
REM Use git submodule instead (recommended)
git submodule update --init --recursive

REM Verify submodule is present
dir third_party\quickfix

REM Build will automatically compile QuickFIX from source


**Alternative Solution (Manual Build):**
.. code-block:: cmd

REM Clone and build QuickFIX manually
git clone https://github.com/quickfix/quickfix.git
cd quickfix
mkdir build && cd build

REM Configure for Windows
cmake .. ^
    -G "Visual Studio 16 2019" ^
    -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DHAVE_SSL=OFF ^
    -DHAVE_MYSQL=OFF ^
    -DHAVE_POSTGRESQL=OFF

REM Build and install
cmake --build . --config Release
cmake --install . --prefix C:\quickfix


Issue 3: Intel TBB Installation Issues
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: cmd

Could not find TBB library
TBB not available for this platform


**Solution:**
.. code-block:: cmd

REM Install Intel TBB via vcpkg
.\vcpkg install tbb:x64-windows

REM Or download from Intel
REM https://github.com/oneapi-src/oneTBB/releases

REM Extract and set environment variables
set TBB_ROOT=C:\tbb
set PATH=%PATH%;%TBB_ROOT%\bin


Build Configuration Issues
--------------------------

Issue 1: CMake Configuration Fails
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: cmd

CMake Error: Could not find CMAKE_CXX_COMPILER
No CMAKE_C_COMPILER could be found


**Solution:**
.. code-block:: cmd

REM Ensure you're in Developer Command Prompt
"C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM Or specify compiler explicitly
cmake .. ^
    -G "Visual Studio 16 2019" ^
    -A x64 ^
    -DCMAKE_CXX_COMPILER=cl ^
    -DCMAKE_C_COMPILER=cl

REM Check Visual Studio installation
where cl
cl


Issue 2: Architecture Mismatch
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: cmd

LNK2019: unresolved external symbol
Architecture mismatch between libraries


**Solution:**
.. code-block:: cmd

REM Ensure consistent architecture (x64)
cmake .. ^
    -G "Visual Studio 16 2019" ^
    -A x64 ^
    -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake

REM Verify vcpkg triplet
.\vcpkg list | findstr x64-windows

REM Reinstall packages if needed
.\vcpkg remove boost:x64-windows
.\vcpkg install boost:x64-windows


Issue 3: Linker Errors
~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: cmd

LINK : fatal error LNK1104: cannot open file 'boost_system-vc142-mt-x64-1_82.lib'


**Solution:**
.. code-block:: cmd

REM Check library paths
dir C:\vcpkg\installed\x64-windows\lib

REM Verify vcpkg integration
.\vcpkg integrate install

REM Clean and rebuild
cmake --build . --target clean
cmake --build . --config Release

REM If still failing, specify library paths manually
cmake .. ^
    -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake ^
    -DCMAKE_PREFIX_PATH=C:\vcpkg\installed\x64-windows


Runtime Issues
--------------

Issue 1: DLL Not Found
~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: cmd

The program can't start because boost_system-vc142-mt-x64-1_82.dll is missing


**Solution:**
.. code-block:: cmd

REM Add vcpkg bin directory to PATH
set PATH=%PATH%;C:\vcpkg\installed\x64-windows\bin

REM Or copy DLLs to executable directory
copy C:\vcpkg\installed\x64-windows\bin\*.dll bin\Release\

REM For permanent solution, use static linking
cmake .. ^
    -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake ^
    -DVCPKG_TARGET_TRIPLET=x64-windows-static

REM Check DLL dependencies
dumpbin /dependents bin\Release\fix-fasttrade.exe


Issue 2: Configuration File Access
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: cmd

Cannot open configuration file: Access is denied
Configuration file not found


**Solution:**
.. code-block:: cmd

REM Check file permissions
icacls config\fix-config.xml

REM Grant read permissions
icacls config\fix-config.xml /grant Users:R

REM Run as administrator if needed
runas /user:Administrator "bin\Release\fix-fasttrade.exe --config config\fix-config.xml"

REM Or copy config to user directory
copy config\fix-config.xml %USERPROFILE%\
bin\Release\fix-fasttrade.exe --config %USERPROFILE%\fix-config.xml


Issue 3: Port Binding Issues
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: cmd

bind: Only one usage of each socket address is normally permitted
Port already in use


**Solution:**
.. code-block:: cmd

REM Check which process is using the port
netstat -ano | findstr :9876

REM Kill the process if needed
taskkill /PID <process_id> /F

REM Or change port in configuration
REM Edit config\fix-config.xml and change port to different value

REM For privileged ports (< 1024), run as administrator
runas /user:Administrator "bin\Release\fix-fasttrade.exe"


Performance Issues
------------------

Issue 1: High CPU Usage
~~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: cmd

CPU usage at 100%
System becomes unresponsive


**Solution:**
.. code-block:: cmd

REM Monitor CPU usage
tasklist /fi "imagename eq fix-fasttrade.exe"

REM Set CPU affinity to specific cores
start /affinity 3 bin\Release\fix-fasttrade.exe --config config\fix-config.xml

REM Or use PowerShell
powershell "Start-Process -FilePath 'bin\Release\fix-fasttrade.exe' -ArgumentList '--config config\fix-config.xml' -ProcessorAffinity 3"

REM Set process priority
start /high bin\Release\fix-fasttrade.exe --config config\fix-config.xml


Issue 2: Memory Issues
~~~~~~~~~~~~~~~~~~~~~~

**Symptoms:**
.. code-block:: cmd

Out of memory error
Virtual memory exhausted


**Solution:**
.. code-block:: cmd

REM Increase virtual memory (page file)
REM Control Panel > System > Advanced > Performance Settings > Advanced > Virtual Memory

REM Monitor memory usage
tasklist /fi "imagename eq fix-fasttrade.exe" /fo table

REM Use memory profiling tools
REM Visual Studio Diagnostic Tools
REM Application Verifier
REM VMMap from Sysinternals


Windows-Specific Optimizations
------------------------------

High-Performance Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmd

REM Build with maximum optimizations
cmake .. ^
    -G "Visual Studio 16 2019" ^
    -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DENABLE_SIMD=ON ^
    -DCMAKE_CXX_FLAGS="/O2 /arch:AVX2 /GL" ^
    -DCMAKE_EXE_LINKER_FLAGS="/LTCG"

cmake --build . --config Release --parallel


Runtime Performance Tuning
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmd

REM Set high process priority
start /realtime bin\Release\fix-fasttrade.exe --config config\fix-config.xml

REM Set CPU affinity (cores 0-3)
start /affinity F bin\Release\fix-fasttrade.exe --config config\fix-config.xml

REM Disable Windows Defender real-time protection for build directory
REM (temporarily, for development only)
powershell "Add-MpPreference -ExclusionPath 'C:\path\to\FIX-FastTrade'"


Windows Firewall Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmd

REM Allow FIX-FastTrade through Windows Firewall
netsh advfirewall firewall add rule name="FIX-FastTrade" dir=in action=allow program="C:\path\to\FIX-FastTrade\bin\Release\fix-fasttrade.exe"

REM Or open specific port
netsh advfirewall firewall add rule name="FIX Port 9876" dir=in action=allow protocol=TCP localport=9876


Debugging Tools
---------------

Using Visual Studio Debugger
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmd

REM Debug with Visual Studio
devenv bin\Release\fix-fasttrade.exe

REM Or use command line debugger
cdb -o bin\Release\fix-fasttrade.exe --config config\fix-config.xml


Using Windows Performance Tools
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmd

REM Install Windows Performance Toolkit
REM Download from: https://docs.microsoft.com/en-us/windows-hardware/test/wpt/

REM Profile CPU usage
wpr -start CPU

REM Run your application
bin\Release\fix-fasttrade.exe --config config\fix-config.xml

REM Stop profiling and analyze
wpr -stop trace.etl
wpa trace.etl


Using Process Monitor
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmd

REM Download Process Monitor from Sysinternals
REM https://docs.microsoft.com/en-us/sysinternals/downloads/procmon

REM Monitor file/registry access
procmon.exe /AcceptEula /Minimized /BackingFile C:\temp\fix-fasttrade.pml

REM Filter by process name: fix-fasttrade.exe


Windows Subsystem for Linux (WSL)
---------------------------------

Using WSL for Development
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

# Install WSL2 with Ubuntu
wsl --install -d Ubuntu

# In WSL, follow Linux build instructions
sudo apt update
sudo apt install -y build-essential cmake pkg-config libboost-all-dev

# Clone and build
git clone --recursive https://github.com/muditbhargava66/FIX-FastTrade.git
cd FIX-FastTrade
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_SIMD=ON
make -j$(nproc)

# Run in WSL
./bin/fix-fasttrade --config ../config/fix-config.xml


Getting Help
------------

Information Collection
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmd

REM System information
systeminfo
wmic cpu get name
wmic computersystem get TotalPhysicalMemory

REM Compiler information
cl
cmake --version
git --version

REM Library information
.\vcpkg list
dir C:\vcpkg\installed\x64-windows\lib


Build Logs
~~~~~~~~~~

.. code-block:: cmd

REM CMake configuration log
cmake .. 2>&1 | tee cmake_config.log

REM Build with verbose output
cmake --build . --config Release --verbose 2>&1 | tee build.log

REM Runtime logs
bin\Release\fix-fasttrade.exe --config config\fix-config.xml --log-level debug 2>&1 | tee runtime.log


Event Viewer
~~~~~~~~~~~~

.. code-block:: cmd

REM Check Windows Event Viewer for application errors
eventvwr.msc

REM Look in:
REM - Windows Logs > Application
REM - Windows Logs > System
REM - Applications and Services Logs


This comprehensive Windows troubleshooting guide should help resolve most issues encountered when building and running FIX-FastTrade on Windows systems.