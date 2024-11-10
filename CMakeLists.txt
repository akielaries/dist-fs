# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(dist-fs)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
    -Wall                    # Enable all the warnings
    -Wextra                  # Enable extra warnings
    -pedantic                # Enable stricter standards compliance
    -Wshadow                 # Warn about variable shadowing
    -Wconversion             # Warn about type conversions that may lose data
    -Wuninitialized          # Warn about uninitialized variables
    -Wmaybe-uninitialized    # Warn about variables that may be uninitialized
    -Wcast-align             # Warn about potentially problematic pointer casts
    -Wunused-variable        # Warn about unused variables
    -Wunused-parameter       # Warn about unused function parameters
    -Wnull-dereference       # Warn about dereferencing a null pointer
)

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

