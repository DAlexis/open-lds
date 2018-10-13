cmake_minimum_required(VERSION 2.8)

project(libdfmath-unit-tests)

find_package (MySQL REQUIRED)
find_package (Boost COMPONENTS date_time REQUIRED)

include_directories (
    ${df-math_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIR}
)

set(EXE_SOURCES
    positioning-test.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME}
    gtest
    gtest_main
    ${df-math_LIBRARY}
    positioning-utils
    timestamp-utils
    math-utils
    constants
    ${MYSQL_LIBRARIES}
    ${Boost_DATE_TIME_LIBRARY}
)

add_test(NAME DFMathTests
         COMMAND ${PROJECT_NAME})
run_test(${PROJECT_NAME})
