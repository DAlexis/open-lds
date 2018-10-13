cmake_minimum_required(VERSION 2.8)

project(mysql-utils-unit-tests)

find_package (MySQL REQUIRED)

include_directories(
    ${mysql-wrapper_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIR}
)

set(EXE_SOURCES
    insert-generator-tests.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries(${PROJECT_NAME}
    gtest
    gtest_main
    ${mysql-wrapper_LIBRARY}
    ${MYSQL_LIBRARY}
)

add_test(NAME MysqlUtilsTests
         COMMAND ${PROJECT_NAME})
run_test(${PROJECT_NAME})
