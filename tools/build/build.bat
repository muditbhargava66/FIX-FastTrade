@echo off
REM FIX-FastTrade Windows Build Script
REM This script builds the FIX-FastTrade project on Windows with proper error handling

setlocal enabledelayedexpansion

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

REM Set the project root directory
set "PROJECT_ROOT=%~dp0..\.."

REM Verify we're in the right directory
if not exist "%PROJECT_ROOT%\CMakeLists.txt" (
    call :print_error "CMakeLists.txt not found in project root: %PROJECT_ROOT%"
    call :print_error "Please ensure this script is in the correct location"
    exit /b 1
)

REM Set the build directory
set "BUILD_DIR=%PROJECT_ROOT%\build"

REM Default settings
set "CMAKE_GENERATOR=Visual Studio 16 2019"
set "BUILD_CONFIG=Release"
set "ARCHITECTURE=x64"
set "PARALLEL_JOBS=%NUMBER_OF_PROCESSORS%"
set "RUN_TESTS=false"
set "CLEAN_BUILD=false"
set "VERBOSE=false"

REM Parse command line arguments
:parse_args
if "%~1"=="" goto :args_done
if /i "%~1"=="-d" set "BUILD_CONFIG=Debug" & shift & goto :parse_args
if /i "%~1"=="--debug" set "BUILD_CONFIG=Debug" & shift & goto :parse_args
if /i "%~1"=="-r" set "BUILD_CONFIG=Release" & shift & goto :parse_args
if /i "%~1"=="--release" set "BUILD_CONFIG=Release" & shift & goto :parse_args
if /i "%~1"=="-t" set "RUN_TESTS=true" & shift & goto :parse_args
if /i "%~1"=="--test" set "RUN_TESTS=true" & shift & goto :parse_args
if /i "%~1"=="-c" set "CLEAN_BUILD=true" & shift & goto :parse_args
if /i "%~1"=="--clean" set "CLEAN_BUILD=true" & shift & goto :parse_args
if /i "%~1"=="-v" set "VERBOSE=true" & shift & goto :parse_args
if /i "%~1"=="--verbose" set "VERBOSE=true" & shift & goto :parse_args
if /i "%~1"=="-h" goto :show_help
if /i "%~1"=="--help" goto :show_help
if /i "%~1"=="-j" set "PARALLEL_JOBS=%~2" & shift & shift & goto :parse_args
if /i "%~1"=="--jobs" set "PARALLEL_JOBS=%~2" & shift & shift & goto :parse_args
if /i "%~1"=="-g" set "CMAKE_GENERATOR=%~2" & shift & shift & goto :parse_args
if /i "%~1"=="--generator" set "CMAKE_GENERATOR=%~2" & shift & shift & goto :parse_args

call :print_error "Unknown option: %~1"
exit /b 1

:show_help
echo Usage: %~nx0 [OPTIONS]
echo Options:
echo   -d, --debug       Build in Debug mode
echo   -r, --release     Build in Release mode (default)
echo   -j, --jobs N      Use N parallel jobs (default: auto-detect)
echo   -t, --test        Run tests after building
echo   -c, --clean       Clean build directory before building
echo   -v, --verbose     Verbose build output
echo   -g, --generator   CMake generator (default: Visual Studio 16 2019)
echo   -h, --help        Show this help message
exit /b 0

:args_done

call :print_status "Build configuration:"
call :print_status "  Build Config: %BUILD_CONFIG%"
call :print_status "  Generator: %CMAKE_GENERATOR%"
call :print_status "  Architecture: %ARCHITECTURE%"
call :print_status "  Parallel Jobs: %PARALLEL_JOBS%"
call :print_status "  Run Tests: %RUN_TESTS%"
call :print_status "  Clean Build: %CLEAN_BUILD%"

REM Clean build directory if requested
if "%CLEAN_BUILD%"=="true" (
    call :print_status "Cleaning build directory..."
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
)

REM Create the build directory if it doesn't exist
call :print_status "Creating build directory..."
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if errorlevel 1 (
    call :print_error "Failed to create build directory: %BUILD_DIR%"
    exit /b 1
)

REM Change to the build directory
cd /d "%BUILD_DIR%"
if errorlevel 1 (
    call :print_error "Failed to change to build directory: %BUILD_DIR%"
    exit /b 1
)

REM Check for vcpkg toolchain
set "CMAKE_ARGS=-G "%CMAKE_GENERATOR%" -A %ARCHITECTURE%"
if exist "%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" (
    set "CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"
    call :print_status "Using vcpkg toolchain"
)

REM Configure the project with CMake
call :print_status "Configuring CMake..."
if "%VERBOSE%"=="true" (
    cmake %CMAKE_ARGS% "%PROJECT_ROOT%"
) else (
    cmake %CMAKE_ARGS% "%PROJECT_ROOT%" > cmake_config.log 2>&1
)

if errorlevel 1 (
    call :print_error "CMake configuration failed"
    if "%VERBOSE%"=="false" if exist cmake_config.log (
        call :print_status "CMake configuration log:"
        type cmake_config.log
    )
    exit /b 1
)

call :print_success "CMake configuration completed"

REM Build the project
call :print_status "Building project with %PARALLEL_JOBS% parallel jobs..."
set "BUILD_ARGS=--build . --config %BUILD_CONFIG% --parallel %PARALLEL_JOBS%"

if "%VERBOSE%"=="true" (
    set "BUILD_ARGS=%BUILD_ARGS% --verbose"
)

cmake %BUILD_ARGS%
if errorlevel 1 (
    call :print_error "Build failed"
    exit /b 1
)

call :print_success "Build completed successfully"

REM Show build artifacts
call :print_status "Build artifacts:"
if exist "bin\%BUILD_CONFIG%" (
    dir "bin\%BUILD_CONFIG%"
) else if exist "bin" (
    dir "bin"
) else (
    call :print_warning "No bin directory found"
)

REM Run tests if requested
if "%RUN_TESTS%"=="true" (
    call :print_status "Running tests..."
    ctest -C "%BUILD_CONFIG%" --output-on-failure --parallel %PARALLEL_JOBS%
    if errorlevel 1 (
        call :print_warning "Some tests failed"
        exit /b 1
    ) else (
        call :print_success "All tests passed"
    )
)

call :print_success "Build script completed successfully"

REM Show next steps
echo.
call :print_status "Next steps:"
if exist "bin\%BUILD_CONFIG%\fix-fasttrade.exe" (
    echo   .\bin\%BUILD_CONFIG%\fix-fasttrade.exe
) else if exist "bin\fix-fasttrade.exe" (
    echo   .\bin\fix-fasttrade.exe
) else (
    echo   Check the bin\ directory for executables
)

if "%RUN_TESTS%"=="false" (
    echo   To run tests: ctest -C %BUILD_CONFIG% --output-on-failure
)

echo   To install: cmake --build . --target install