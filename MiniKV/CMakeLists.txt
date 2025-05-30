cmake_minimum_required(VERSION 3.14)
project(MiniKV VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler flags
# Platform-specific compiler flags
if(MSVC)
    # Visual Studio flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    # GCC/Clang flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
endif()

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/kvstore.cpp
    src/wal.cpp
    src/sstable.cpp
    src/utils.cpp
)

# Create library
add_library(minikv_lib ${SOURCES})

# Main executable
add_executable(minikv src/main.cpp)
target_link_libraries(minikv minikv_lib)

# Google Test
find_package(GTest QUIET)
if(GTest_FOUND)
    enable_testing()
    
    # Test files
    set(TEST_SOURCES
        test/kvstore_test.cpp
        test/wal_test.cpp
        test/sstable_test.cpp
    )
    
    # Create test executable
    add_executable(minikv_tests ${TEST_SOURCES})
    target_link_libraries(minikv_tests minikv_lib GTest::gtest_main)
    
    # Register tests
    include(GoogleTest)
    gtest_discover_tests(minikv_tests)
else()
    message(WARNING "Google Test not found. Tests will not be built.")
endif()

# Install targets
install(TARGETS minikv DESTINATION bin)
