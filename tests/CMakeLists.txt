cmake_minimum_required(VERSION 3.10)

# GoogleTest configuration with multiple fallback strategies
set(USE_FETCHCONTENT_GTEST FALSE)

# Determine GoogleTest strategy
if(FORCE_FETCHCONTENT_GTEST)
    message(STATUS "Forced to use FetchContent for GoogleTest")
    set(USE_FETCHCONTENT_GTEST TRUE)
elseif(APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
    message(STATUS "Detected macOS ARM64 - Using FetchContent for GoogleTest to avoid linking issues")
    set(USE_FETCHCONTENT_GTEST TRUE)
else()
    # Try to find system GoogleTest first
    find_package(GTest QUIET)
    find_package(GMock QUIET)
    
    if(NOT GTest_FOUND)
        message(STATUS "System GoogleTest not found - Using FetchContent")
        set(USE_FETCHCONTENT_GTEST TRUE)
    endif()
endif()

# Configure GoogleTest based on strategy
if(USE_FETCHCONTENT_GTEST)
    message(STATUS "Building GoogleTest from source using FetchContent")
    
    include(FetchContent)
    
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
        GIT_SHALLOW    TRUE
    )
    
    # Configure GoogleTest options
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
    set(BUILD_GTEST ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(googletest)
    
    # Fix compiler flags for GoogleTest on macOS ARM64
    if(APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
        target_compile_options(gtest PRIVATE -Wno-nan-infinity-disabled)
        target_compile_options(gtest_main PRIVATE -Wno-nan-infinity-disabled)
        target_compile_options(gmock PRIVATE -Wno-nan-infinity-disabled)
        target_compile_options(gmock_main PRIVATE -Wno-nan-infinity-disabled)
    endif()
    
    # Set GoogleTest targets for FetchContent
    set(GTEST_LIBRARIES gtest gtest_main gmock gmock_main)
    set(GTEST_FOUND TRUE)
    
else()
    message(STATUS "Using system GoogleTest")
    # Use system GoogleTest
    set(GTEST_LIBRARIES GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
endif()

# Only build tests if GoogleTest is available
if(GTEST_FOUND)
    message(STATUS "Building tests with GoogleTest")
    
    # Include directories
    include_directories(
        ${CMAKE_SOURCE_DIR}/include
    )
    
    # Test source files
    file(GLOB_RECURSE TEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )
    
    # Test executable target
    add_executable(fix-fasttrade-tests ${TEST_SOURCES})
    
    # Set target properties
    set_target_properties(fix-fasttrade-tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    
    # Link libraries with proper error handling
    target_link_libraries(fix-fasttrade-tests
        PRIVATE
            fix-fasttrade-core
            ${GTEST_LIBRARIES}
            Threads::Threads
    )
    
    # Add compiler flags for tests
    target_compile_options(fix-fasttrade-tests PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-unused-parameter>
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-unused-variable>
    )
    
    # Add tests to CTest
    include(GoogleTest)
    gtest_discover_tests(fix-fasttrade-tests
        TIMEOUT 30
        DISCOVERY_TIMEOUT 10
    )
    
    message(STATUS "Tests configured successfully")
else()
    message(WARNING "GoogleTest not found - tests will not be built")
    message(STATUS "To build tests, install GoogleTest or enable FetchContent")
endif()