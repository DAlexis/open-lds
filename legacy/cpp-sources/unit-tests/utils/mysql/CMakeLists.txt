cmake_minimum_required(VERSION 3.1)

project(mysql-utils-unit-tests)

find_package (MySQL REQUIRED)

set(EXE_SOURCES
    insert-generator-tests.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC
    gtest
    gtest_main
    mysql-wrapper
)

add_test(NAME MysqlUtilsTests
         COMMAND ${PROJECT_NAME})
