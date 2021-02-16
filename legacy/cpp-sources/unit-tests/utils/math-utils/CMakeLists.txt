cmake_minimum_required(VERSION 3.1)

project(math-utils-unit-tests)

    #${GTEST_INCLUDE_DIR}


set(EXE_SOURCES
    linear-algebra-test.cpp
    gradient-solver-test.cpp
    clasterisation-solver-test.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME} PUBLIC
    gtest
    gtest_main
    math-utils
)

add_test(NAME MathUtilsTests
         COMMAND ${PROJECT_NAME})

