cmake_minimum_required(VERSION 3.10)

enable_testing()

find_package(GTest REQUIRED)

file(GLOB_RECURSE UNIT_TEST_SOURCES *.cpp)



set(DIST_FS_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/dist-fs/storage_driver.cpp
    ${CMAKE_SOURCE_DIR}/dist-fs/audio_files.cpp
    ${CMAKE_SOURCE_DIR}/dist-fs/utils.cpp
    ${CMAKE_SOURCE_DIR}/dist-fs/config.cpp
)

add_executable(unit_tests ${UNIT_TEST_SOURCES} ${DIST_FS_TEST_SOURCES})

target_link_libraries(unit_tests PRIVATE GTest::GTest GTest::Main)
target_include_directories(unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/dist-fs)

add_test(NAME unit_tests COMMAND unit_tests)
