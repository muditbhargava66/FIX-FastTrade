cmake_minimum_required(VERSION 3.14)

project(FIX-FastTrade VERSION 2.0.0 LANGUAGES CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific optimization flags for Phase 1 performance
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Aggressive optimizations for release builds
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -mtune=native -flto -funroll-loops -ffast-math -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -DDEBUG -fsanitize=address -fno-omit-frame-pointer")
    
    # Enable all warnings but be more lenient with third-party code
    add_compile_options(-Wall -Wextra -Wpedantic)
    
    # Only enable -Werror for our own code, not third-party dependencies
    # This will be applied per-target later
    
    # Enable link-time optimization
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
elseif(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /GL /DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi /DDEBUG")
    add_compile_options(/W4)
    # Remove /WX for now to avoid third-party issues
endif()

# Set the output directories for binary files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include modern dependency management
include(cmake/Dependencies.cmake)

# Thread support
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# Add third-party include directories if using submodules
if(EXISTS "${CMAKE_SOURCE_DIR}/third_party/quickfix/include")
    include_directories(${CMAKE_SOURCE_DIR}/third_party/quickfix/include)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/third_party/spdlog/include")
    include_directories(${CMAKE_SOURCE_DIR}/third_party/spdlog/include)
endif()

# Collect source files by component
# Phase 1: Infrastructure sources
file(GLOB_RECURSE INFRASTRUCTURE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/infrastructure/*.cpp
)

# Existing FIX sources
file(GLOB_RECURSE FIX_SOURCES
    ${CMAKE_SOURCE_DIR}/src/fix/*.cpp
)

# Existing model sources
file(GLOB_RECURSE MODEL_SOURCES
    ${CMAKE_SOURCE_DIR}/src/model/*.cpp
)

# Existing service sources
file(GLOB_RECURSE SERVICE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/service/*.cpp
)

# Main executable source
set(MAIN_SOURCE ${CMAKE_SOURCE_DIR}/src/main.cpp)

# Create main executable
add_executable(fix-fasttrade 
    ${MAIN_SOURCE}
    ${INFRASTRUCTURE_SOURCES}
    ${FIX_SOURCES}
    ${MODEL_SOURCES}
    ${SERVICE_SOURCES}
)

# Set target properties
set_target_properties(fix-fasttrade PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# Add strict warnings only to our own code
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(fix-fasttrade PRIVATE -Werror)
elseif(MSVC)
    target_compile_options(fix-fasttrade PRIVATE /WX)
endif()

# Link libraries - modern CMake approach
target_link_libraries(fix-fasttrade
    PRIVATE
        Threads::Threads
)

# Link Boost
if(TARGET Boost::system)
    target_link_libraries(fix-fasttrade PRIVATE 
        Boost::system Boost::thread Boost::filesystem 
        Boost::date_time Boost::program_options Boost::log)
elseif(Boost_FOUND)
    target_link_libraries(fix-fasttrade PRIVATE ${Boost_LIBRARIES})
    target_include_directories(fix-fasttrade PRIVATE ${Boost_INCLUDE_DIRS})
endif()

# Link QuickFIX
if(TARGET quickfix::quickfix)
    target_link_libraries(fix-fasttrade PRIVATE quickfix::quickfix)
elseif(QUICKFIX_LIBRARY)
    target_link_libraries(fix-fasttrade PRIVATE ${QUICKFIX_LIBRARY})
    target_include_directories(fix-fasttrade PRIVATE ${QUICKFIX_INCLUDE_DIR})
endif()

# Link TBB if available
if(TARGET TBB::tbb)
    target_link_libraries(fix-fasttrade PRIVATE TBB::tbb)
endif()

# Link spdlog if available
if(TARGET spdlog::spdlog)
    target_link_libraries(fix-fasttrade PRIVATE spdlog::spdlog)
endif()

# Platform-specific libraries
if(UNIX AND NOT APPLE)
    target_link_libraries(fix-fasttrade PRIVATE rt)  # For high-resolution timers
endif()

# Create static library for reuse
add_library(fix-fasttrade-core STATIC
    ${INFRASTRUCTURE_SOURCES}
    ${FIX_SOURCES}
    ${MODEL_SOURCES}
    ${SERVICE_SOURCES}
)

# Add strict warnings only to our own code
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(fix-fasttrade-core PRIVATE -Werror)
elseif(MSVC)
    target_compile_options(fix-fasttrade-core PRIVATE /WX)
endif()

target_link_libraries(fix-fasttrade-core
    PUBLIC
        ${Boost_LIBRARIES}
        ${QUICKFIX_LIBRARY}
        Threads::Threads
        $<$<BOOL:${TBB_FOUND}>:TBB::tbb>
)

# Testing options
option(BUILD_TESTS "Build the test suite" ON)
option(FORCE_FETCHCONTENT_GTEST "Force building GoogleTest from source instead of using system version" OFF)

# Add test target
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
else()
    message(STATUS "Tests disabled - use -DBUILD_TESTS=ON to enable")
endif()

# Performance benchmarks (if Google Benchmark is available)
if(benchmark_FOUND)
    add_subdirectory(benchmarks)
endif()

# Generate documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_EXTRACT_PRIVATE YES)
    set(DOXYGEN_EXTRACT_STATIC YES)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_SOURCE_DIR}/README.md)
    set(DOXYGEN_EXCLUDE_PATTERNS */third_party/* */build/*)
    
    doxygen_add_docs(docs
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/README.md
        COMMENT "Generating API documentation with Doxygen"
    )
endif()

# Install targets
install(TARGETS fix-fasttrade fix-fasttrade-core
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install header files
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ 
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Install configuration files
install(DIRECTORY ${CMAKE_SOURCE_DIR}/config/ 
    DESTINATION config
    FILES_MATCHING PATTERN "*.json" PATTERN "*.xml" PATTERN "*.conf"
)

# Install documentation
if(DOXYGEN_FOUND)
    install(DIRECTORY ${CMAKE_BINARY_DIR}/docs/html/ 
        DESTINATION docs
        OPTIONAL
    )
endif()

# CPack configuration for packaging
set(CPACK_GENERATOR "ZIP;TGZ;DEB;RPM")
set(CPACK_PACKAGE_NAME "FIX-FastTrade")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "Ultra-low latency FIX protocol trading system with Phase 1 optimizations")
set(CPACK_PACKAGE_CONTACT "FIX-FastTrade Team")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

# Debian specific
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-all-dev (>= 1.71)")
set(CPACK_DEBIAN_PACKAGE_SECTION "finance")

# RPM specific
set(CPACK_RPM_PACKAGE_REQUIRES "boost >= 1.71")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Finance")

include(CPack)

# Print configuration summary
message(STATUS "")
message(STATUS "FIX-FastTrade Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Boost Version: ${Boost_VERSION}")
message(STATUS "  QuickFIX: Found at ${QUICKFIX_INCLUDE_DIR}")
message(STATUS "  Intel TBB: ${TBB_FOUND}")
message(STATUS "  Google Benchmark: ${benchmark_FOUND}")
message(STATUS "  Documentation: ${DOXYGEN_FOUND}")
message(STATUS "")
