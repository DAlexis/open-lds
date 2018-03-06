cmake_minimum_required(VERSION 2.8)

project(math-utils-unit-tests)

find_package (GTest REQUIRED)

include_directories (
    ${math-utils_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIR}
)

set(EXE_SOURCES
    linear-algebra-test.cpp
    gradient-solver-test.cpp
    clasterisation-solver-test.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME}
    gtest
    gtest_main
    ${math-utils_LIBRARY}
)

add_test(NAME MathUtilsTests
         COMMAND ${PROJECT_NAME})
run_test(${PROJECT_NAME})
