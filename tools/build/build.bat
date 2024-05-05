@echo off

REM Set the project root directory
set PROJECT_ROOT=%~dp0..\..\

REM Set the build directory
set BUILD_DIR=%PROJECT_ROOT%\build

REM Set the CMake generator (adjust as needed)
set CMAKE_GENERATOR=Visual Studio 16 2019

REM Set the build configuration (Release, Debug, etc.)
set BUILD_CONFIG=Release

REM Create the build directory if it doesn't exist
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Change to the build directory
cd "%BUILD_DIR%"

REM Configure the project with CMake
cmake -G "%CMAKE_GENERATOR%" -A x64 -DCMAKE_BUILD_TYPE="%BUILD_CONFIG%" "%PROJECT_ROOT%"

REM Build the project
cmake --build . --config "%BUILD_CONFIG%"

REM Run the tests (if any)
ctest -C "%BUILD_CONFIG%" --output-on-failure