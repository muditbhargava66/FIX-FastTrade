@echo off
REM Windows-specific setup script for FIX-FastTrade
REM This script handles Windows-specific dependency installation

setlocal enabledelayedexpansion

echo Starting Windows-specific setup for FIX-FastTrade...

REM Colors for output (limited in batch)
set "INFO_COLOR=[94m"
set "SUCCESS_COLOR=[92m"
set "WARNING_COLOR=[93m"
set "ERROR_COLOR=[91m"
set "RESET_COLOR=[0m"

REM Function to print colored output (simulated)
:print_status
echo %INFO_COLOR%[INFO]%RESET_COLOR% %~1
goto :eof

:print_success
echo %SUCCESS_COLOR%[SUCCESS]%RESET_COLOR% %~1
goto :eof

:print_warning
echo %WARNING_COLOR%[WARNING]%RESET_COLOR% %~1
goto :eof

:print_error
echo %ERROR_COLOR%[ERROR]%RESET_COLOR% %~1
goto :eof

REM Check if we're in the right directory
if not exist "CMakeLists.txt" (
    call :print_error "Please run this script from the FIX-FastTrade root directory"
    exit /b 1
)

REM Detect Windows architecture
set "ARCH=x64"
if "%PROCESSOR_ARCHITECTURE%"=="x86" set "ARCH=x86"
if "%PROCESSOR_ARCHITEW6432%"=="AMD64" set "ARCH=x64"

call :print_status "Detected Windows architecture: %ARCH%"

REM Check for required tools
call :print_status "Checking for required tools..."

REM Check for Visual Studio or Build Tools
set "VS_FOUND=0"
if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2019" set "VS_FOUND=1"
if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2022" set "VS_FOUND=1"
if exist "%ProgramFiles%\Microsoft Visual Studio\2019" set "VS_FOUND=1"
if exist "%ProgramFiles%\Microsoft Visual Studio\2022" set "VS_FOUND=1"

if "%VS_FOUND%"=="0" (
    call :print_error "Visual Studio 2019 or 2022 not found"
    call :print_status "Please install Visual Studio with C++ development tools"
    call :print_status "Download from: https://visualstudio.microsoft.com/downloads/"
    exit /b 1
) else (
    call :print_success "Visual Studio found"
)

REM Check for CMake
cmake --version >nul 2>&1
if errorlevel 1 (
    call :print_error "CMake not found"
    call :print_status "Please install CMake from: https://cmake.org/download/"
    exit /b 1
) else (
    call :print_success "CMake found"
)

REM Check for Git
git --version >nul 2>&1
if errorlevel 1 (
    call :print_error "Git not found"
    call :print_status "Please install Git from: https://git-scm.com/download/win"
    exit /b 1
) else (
    call :print_success "Git found"
)

REM Check for vcpkg (recommended package manager for Windows)
call :print_status "Checking for vcpkg..."
if exist "vcpkg.exe" (
    call :print_success "vcpkg found in current directory"
    set "VCPKG_ROOT=%CD%"
) else if exist "%VCPKG_ROOT%\vcpkg.exe" (
    call :print_success "vcpkg found at %VCPKG_ROOT%"
) else (
    call :print_warning "vcpkg not found"
    call :print_status "vcpkg is recommended for managing C++ dependencies on Windows"
    call :print_status "Install vcpkg from: https://github.com/Microsoft/vcpkg"
    
    REM Ask user if they want to install vcpkg
    set /p "INSTALL_VCPKG=Do you want to install vcpkg? (y/n): "
    if /i "!INSTALL_VCPKG!"=="y" (
        call :install_vcpkg
    )
)

REM Install dependencies
call :print_status "Installing dependencies..."

if exist "%VCPKG_ROOT%\vcpkg.exe" (
    call :install_vcpkg_dependencies
) else (
    call :print_warning "Installing dependencies manually..."
    call :install_manual_dependencies
)

REM Setup build environment
call :print_status "Setting up build environment..."

REM Create build directory
if not exist "build" mkdir build

REM Configure CMake
call :print_status "Configuring CMake..."
cd build

set "CMAKE_ARGS=-G "Visual Studio 16 2019" -A %ARCH%"
if exist "%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" (
    set "CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"
)

cmake .. %CMAKE_ARGS%
if errorlevel 1 (
    call :print_error "CMake configuration failed"
    cd ..
    exit /b 1
)

cd ..

call :print_success "Windows setup complete!"
echo.
echo Next steps:
echo   cd build
echo   cmake --build . --config Release
echo   .\bin\Release\fix-fasttrade.exe
echo.
echo For development:
echo   cmake --build . --config Debug
echo   cmake --build . --target RUN_TESTS
echo.

goto :eof

REM Function to install vcpkg
:install_vcpkg
call :print_status "Installing vcpkg..."
git clone https://github.com/Microsoft/vcpkg.git
if errorlevel 1 (
    call :print_error "Failed to clone vcpkg repository"
    goto :eof
)

cd vcpkg
call bootstrap-vcpkg.bat
if errorlevel 1 (
    call :print_error "Failed to bootstrap vcpkg"
    cd ..
    goto :eof
)

set "VCPKG_ROOT=%CD%"
cd ..
call :print_success "vcpkg installed successfully"
goto :eof

REM Function to install dependencies using vcpkg
:install_vcpkg_dependencies
call :print_status "Installing dependencies using vcpkg..."

REM Core dependencies
call :print_status "Installing Boost..."
"%VCPKG_ROOT%\vcpkg.exe" install boost-system boost-thread boost-filesystem boost-date-time boost-program-options boost-log --triplet=%ARCH%-windows
if errorlevel 1 (
    call :print_error "Failed to install Boost"
    goto :eof
)

REM QuickFIX (if available)
call :print_status "Installing QuickFIX..."
"%VCPKG_ROOT%\vcpkg.exe" install quickfix --triplet=%ARCH%-windows
if errorlevel 1 (
    call :print_warning "QuickFIX not available via vcpkg - will use submodule"
)

REM Optional dependencies
call :print_status "Installing optional dependencies..."
"%VCPKG_ROOT%\vcpkg.exe" install tbb benchmark gtest --triplet=%ARCH%-windows
if errorlevel 1 (
    call :print_warning "Some optional dependencies failed to install"
)

call :print_success "vcpkg dependencies installed"
goto :eof

REM Function to install dependencies manually
:install_manual_dependencies
call :print_warning "Manual dependency installation not fully automated on Windows"
call :print_status "Please ensure the following dependencies are available:"
echo   - Boost libraries (^>= 1.71)
echo   - QuickFIX library (optional)
echo   - Intel TBB (optional)
echo   - Google Test (for testing)
echo   - Google Benchmark (optional)
echo.
call :print_status "Consider using vcpkg for easier dependency management"
goto :eof

REM Function to verify Windows setup
:verify_windows_setup
call :print_status "Verifying Windows setup..."

set "VERIFICATION_FAILED=0"

REM Check Visual Studio
if "%VS_FOUND%"=="1" (
    call :print_success "Visual Studio: OK"
) else (
    call :print_error "Visual Studio: MISSING"
    set /a "VERIFICATION_FAILED+=1"
)

REM Check CMake
cmake --version >nul 2>&1
if errorlevel 1 (
    call :print_error "CMake: MISSING"
    set /a "VERIFICATION_FAILED+=1"
) else (
    call :print_success "CMake: OK"
)

REM Check Git
git --version >nul 2>&1
if errorlevel 1 (
    call :print_error "Git: MISSING"
    set /a "VERIFICATION_FAILED+=1"
) else (
    call :print_success "Git: OK"
)

if "%VERIFICATION_FAILED%"=="0" (
    call :print_success "Windows setup verification passed"
) else (
    call :print_error "%VERIFICATION_FAILED% verification checks failed"
)

goto :eof