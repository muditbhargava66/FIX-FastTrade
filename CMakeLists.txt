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

# Add QuickFIX library
execute_process(
    COMMAND quickfix-config --include-dir
    OUTPUT_VARIABLE QUICKFIX_INCLUDE_DIRS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND quickfix-config --libs --cflags
    OUTPUT_VARIABLE QUICKFIX_LIBRARIES
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT QUICKFIX_LIBRARIES)
    message(FATAL_ERROR "QuickFIX libraries not found")
endif()

# Add GoogleTest library
find_package(GTest REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
    ${QUICKFIX_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
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
    ${QUICKFIX_LIBRARIES}
    ${GTEST_LIBRARIES}
    pthread
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
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_EXCLUDE_PATTERNS */third_party/*)
    configure_file(Doxyfile.in Doxyfile @ONLY)
    add_custom_target(docs
        ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()

# Install target
install(TARGETS fix-fasttrade
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install header files
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include)

# Install configuration files
install(DIRECTORY ${CMAKE_SOURCE_DIR}/config/ DESTINATION config)

# Install documentation
install(DIRECTORY ${CMAKE_BINARY_DIR}/docs/ DESTINATION docs)

# CPack configuration
set(CPACK_GENERATOR "ZIP;TGZ")
set(CPACK_PACKAGE_NAME "FIX-FastTrade")
set(CPACK_PACKAGE_VERSION "1.0.5")
set(CPACK_PACKAGE_DESCRIPTION "High-performance FIX protocol trading system")
set(CPACK_PACKAGE_CONTACT "muditbhargava666@gmail.com")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

include(CPack)