cmake_minimum_required(VERSION 3.1)

project(positioning-utils-unit-tests)

set(EXE_SOURCES
    positioning-tests.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME}
    positioning-utils
    gtest
    gtest_main
)

add_test(NAME MathUtilsTests
         COMMAND ${PROJECT_NAME})

