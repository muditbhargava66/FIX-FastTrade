cmake_minimum_required(VERSION 3.10)

# Add Google Test
find_package(GTest REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${GTEST_INCLUDE_DIRS}
)

# Test source files
file(GLOB_RECURSE TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Test executable target
add_executable(fix-fasttrade-tests ${TEST_SOURCES})

# Link libraries
target_link_libraries(fix-fasttrade-tests
    ${GTEST_LIBRARIES}
    pthread
)

# Add tests
include(GoogleTest)
gtest_discover_tests(fix-fasttrade-tests)