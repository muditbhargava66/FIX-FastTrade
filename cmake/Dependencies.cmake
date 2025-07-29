# Dependencies.cmake - Modern dependency management for FIX-FastTrade

include(FetchContent)
include(ExternalProject)

# Set default build type for dependencies
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Function to add a dependency with fallback options
function(add_dependency_with_fallback name)
    set(options REQUIRED)
    set(oneValueArgs 
        SYSTEM_NAME 
        SUBMODULE_PATH 
        GIT_REPOSITORY 
        GIT_TAG
        FIND_PACKAGE_NAME
    )
    set(multiValueArgs COMPONENTS)
    
    cmake_parse_arguments(DEP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Try 1: System package
    if(DEP_FIND_PACKAGE_NAME)
        find_package(${DEP_FIND_PACKAGE_NAME} QUIET COMPONENTS ${DEP_COMPONENTS})
        if(${DEP_FIND_PACKAGE_NAME}_FOUND)
            message(STATUS "Found system ${name}: ${${DEP_FIND_PACKAGE_NAME}_VERSION}")
            return()
        endif()
    endif()
    
    # Try 2: Git submodule
    if(DEP_SUBMODULE_PATH AND EXISTS "${CMAKE_SOURCE_DIR}/${DEP_SUBMODULE_PATH}/CMakeLists.txt")
        message(STATUS "Using submodule for ${name}: ${DEP_SUBMODULE_PATH}")
        # Handle QuickFIX CMake version issue
        if(name STREQUAL "quickfix")
            set(CMAKE_POLICY_DEFAULT_CMP0000 NEW)
            add_subdirectory(${DEP_SUBMODULE_PATH} EXCLUDE_FROM_ALL)
        else()
            add_subdirectory(${DEP_SUBMODULE_PATH})
        endif()
        return()
    endif()
    
    # Try 3: FetchContent
    if(DEP_GIT_REPOSITORY)
        message(STATUS "Fetching ${name} from ${DEP_GIT_REPOSITORY}")
        FetchContent_Declare(
            ${name}
            GIT_REPOSITORY ${DEP_GIT_REPOSITORY}
            GIT_TAG ${DEP_GIT_TAG}
        )
        FetchContent_MakeAvailable(${name})
        return()
    endif()
    
    # Fail if required
    if(DEP_REQUIRED)
        message(FATAL_ERROR "Could not find or fetch required dependency: ${name}")
    else()
        message(WARNING "Could not find optional dependency: ${name}")
    endif()
endfunction()

# Configure dependencies
message(STATUS "Configuring dependencies...")

# 1. Google Test
add_dependency_with_fallback(googletest
    FIND_PACKAGE_NAME GTest
    SUBMODULE_PATH third_party/googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    REQUIRED
)

# 2. QuickFIX - Handle specially due to CMake version issues
find_path(QUICKFIX_INCLUDE_DIR quickfix/Message.h
    HINTS 
        /usr/local/include 
        /usr/include
        ${CMAKE_SOURCE_DIR}/third_party/quickfix/include
)

find_library(QUICKFIX_LIBRARY 
    NAMES quickfix libquickfix
    HINTS 
        /usr/local/lib 
        /usr/lib
        ${CMAKE_SOURCE_DIR}/third_party/quickfix/lib
        ${CMAKE_SOURCE_DIR}/third_party/quickfix/build/lib
)

if(QUICKFIX_INCLUDE_DIR AND QUICKFIX_LIBRARY)
    message(STATUS "Found QuickFIX: ${QUICKFIX_LIBRARY}")
    add_library(quickfix::quickfix UNKNOWN IMPORTED)
    set_target_properties(quickfix::quickfix PROPERTIES
        IMPORTED_LOCATION "${QUICKFIX_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${QUICKFIX_INCLUDE_DIR}"
    )
else()
    message(STATUS "QuickFIX not found. Please install it or build the submodule manually.")
    message(STATUS "To build submodule: cd third_party/quickfix && mkdir build && cd build && cmake .. && make")
endif()

# 3. Boost (try system first, then submodule)
find_package(Boost 1.71.0 QUIET COMPONENTS 
    system thread filesystem date_time program_options log
)

if(NOT Boost_FOUND)
    if(EXISTS "${CMAKE_SOURCE_DIR}/third_party/boost/CMakeLists.txt")
        message(STATUS "Using Boost submodule")
        set(BOOST_ENABLE_CMAKE ON)
        add_subdirectory(third_party/boost)
    else()
        # Use FetchContent for Boost (this can be slow)
        message(STATUS "Fetching Boost...")
        FetchContent_Declare(
            Boost
            GIT_REPOSITORY https://github.com/boostorg/boost.git
            GIT_TAG boost-1.88.0
        )
        FetchContent_MakeAvailable(Boost)
    endif()
else()
    message(STATUS "Found system Boost: ${Boost_VERSION}")
endif()

# 4. spdlog (modern logging library) - optional
find_package(spdlog QUIET)
if(spdlog_FOUND)
    message(STATUS "Found spdlog")
else()
    message(STATUS "spdlog not found - using Boost.Log instead")
endif()

# 5. Intel TBB (optional)
find_package(TBB QUIET)
if(TBB_FOUND)
    message(STATUS "Found Intel TBB: ${TBB_VERSION}")
    add_definitions(-DHAVE_TBB)
else()
    message(STATUS "Intel TBB not found - continuing without it")
endif()

# 6. Google Benchmark (optional)
find_package(benchmark QUIET)
if(benchmark_FOUND)
    message(STATUS "Found Google Benchmark")
    add_definitions(-DHAVE_BENCHMARK)
else()
    message(STATUS "Google Benchmark not found - continuing without it")
endif()

# Platform-specific dependencies
if(APPLE)
    # macOS specific libraries
    find_library(CORE_FOUNDATION CoreFoundation)
    find_library(SYSTEM_CONFIGURATION SystemConfiguration)
endif()

if(UNIX AND NOT APPLE)
    # Linux specific libraries
    find_package(Threads REQUIRED)
endif()

# Summary
message(STATUS "Dependencies configured:")
message(STATUS "  - GoogleTest: ${GTEST_FOUND}")
message(STATUS "  - QuickFIX: Available")
message(STATUS "  - Boost: ${Boost_FOUND}")
message(STATUS "  - spdlog: Available")
message(STATUS "  - Intel TBB: ${TBB_FOUND}")
message(STATUS "  - Google Benchmark: ${benchmark_FOUND}")