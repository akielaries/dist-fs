cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME dist-fs)
project(${PROJECT_NAME} LANGUAGES CXX)

# set C++ standard
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

# include directories (for header files in dist-fs)
include_directories(dist-fs)

# glob all source files
file(GLOB_RECURSE ALL_C_SOURCES dist-fs/*.c)
SET_SOURCE_FILES_PROPERTIES(${ALL_C_SOURCES} PROPERTIES LANGUAGE CXX)

file(GLOB_RECURSE ALL_CPP_SOURCES dist-fs/*.cpp)

# Add a CMake option to select the target executable
option(BUILD_DIST_FS_SERVER "Build dist-fs_server instead of dist-fs" OFF)

if(BUILD_DIST_FS_SERVER)
    # Exclude dist-fs.cpp for dist-fs_server executable
    list(FILTER ALL_CPP_SOURCES EXCLUDE REGEX ".*dist-fs\\.cpp$")
    list(FILTER ALL_CPP_SOURCES EXCLUDE REGEX ".*client\\.cpp$")
    
    # Define source files for dist-fs_server executable
    set(DIST_FS_SERVER_DRIVER dist-fs/dist-fs_server.cpp)
    set(DIST_FS_SERVER_SOURCES ${ALL_C_SOURCES} ${ALL_CPP_SOURCES} ${DIST_FS_SERVER_DRIVER})

    # Define dist-fs_server executable
    add_executable(dist-fs_server ${DIST_FS_SERVER_SOURCES})
else()
    # Exclude dist-fs_server.cpp for dist-fs executable
    list(FILTER ALL_CPP_SOURCES EXCLUDE REGEX ".*dist-fs_server\\.cpp$")
    list(FILTER ALL_CPP_SOURCES EXCLUDE REGEX ".*client\\.cpp$")
    
    # Define source files for dist-fs executable
    set(DIST_FS_DRIVER dist-fs.cpp)
    set(DIST_FS_SOURCES ${ALL_C_SOURCES} ${ALL_CPP_SOURCES} ${DIST_FS_DRIVER})

    # Define dist-fs executable
    add_executable(dist-fs ${DIST_FS_SOURCES})
endif()

# Optional: Link any additional libraries here
# target_link_libraries(dist-fs <library_name>)
# target_link_libraries(dist-fs_server <library_name>)

