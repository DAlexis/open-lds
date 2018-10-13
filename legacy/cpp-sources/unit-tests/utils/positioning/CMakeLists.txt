cmake_minimum_required(VERSION 2.8)

project(positioning-utils-unit-tests)

include_directories (
    ${positioning-utils_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIR}
)

set(EXE_SOURCES
    positioning-tests.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME}
    ${positioning-utils_LIBRARY}
    gtest
    gtest_main
)

add_test(NAME MathUtilsTests
         COMMAND ${PROJECT_NAME})
run_test(${PROJECT_NAME})
