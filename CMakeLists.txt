cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME dist-fs)
project(${PROJECT_NAME} LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
    -g
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
    #-O3                      # the compiler is better at optimizing than me
)

# C sources
file(GLOB_RECURSE C_SOURCES dist-fs/*.c)
# C++ sources 
file(GLOB_RECURSE CPP_SOURCES dist-fs/*.cpp)
# main driver file
set(DRIVER_SRC dist-fs.cpp)

# Add all source files in the dist-fs directory, including any .c and .cpp files

file(GLOB_RECURSE 
  DIST_FS_SOURCES 
  ${C_SOURCES} 
  ${CPP_SOURCES}
  ${DRIVER_SRC}
)

# Include directories (for header files in dist-fs)
include_directories(dist-fs)

# Create the executable
add_executable(dist-fs ${DIST_FS_SOURCES})

# Optional: Link any additional libraries here
# target_link_libraries(dist-fs <library_name>)

