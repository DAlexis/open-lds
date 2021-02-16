cmake_minimum_required(VERSION 3.1)

project(libdfmath-unit-tests)

set(EXE_SOURCES
    positioning-test.cpp
    time-shift-test.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME}
    gtest
    gtest_main
    -Wl,--whole-archive
    df-math
    -Wl,--no-whole-archive
)

add_test(NAME DFMathTests
         COMMAND ${PROJECT_NAME})
