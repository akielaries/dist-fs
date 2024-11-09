# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(dist-fs)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the root driver file
set(SOURCES
    dist-fs.cpp
)

# Add all source files in the dist-fs directory, including any .cpp files
file(GLOB_RECURSE DIST_FS_SOURCES dist-fs/*.cpp)
list(APPEND SOURCES ${DIST_FS_SOURCES})

# Include directories (for header files in dist-fs)
include_directories(dist-fs)

# Create the executable
add_executable(dist-fs ${SOURCES})

# Optional: Link any additional libraries here
# target_link_libraries(dist-fs <library_name>)

