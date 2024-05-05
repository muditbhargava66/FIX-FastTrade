cmake_minimum_required(VERSION 3.10)
project(FIX-FastTrade CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Set the output directories for binary files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add Boost library
find_package(Boost 1.71.0 REQUIRED COMPONENTS system)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
)

# Source files
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# Executable target
add_executable(fix-fasttrade ${SOURCES})

# Link libraries
target_link_libraries(fix-fasttrade
    ${Boost_LIBRARIES}
)

# Enable warnings and treat them as errors
if(MSVC)
    target_compile_options(fix-fasttrade PRIVATE /W4 /WX)
else()
    target_compile_options(fix-fasttrade PRIVATE -Wall -Wextra -Werror)
endif()

# Add test target
enable_testing()
add_subdirectory(tests)

# Generate documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile)
    add_custom_target(docs
        ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()