cmake_minimum_required(VERSION 3.1)

project(mysql-wrapper)

find_package(MySQL REQUIRED)

set(LIB_SOURCE
    MysqlWrapper.cpp
    MysqlWrapper.hpp
)

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCE})

target_link_libraries(${PROJECT_NAME} PUBLIC ${MYSQL_LIBRARY} blconfig)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR} ${MYSQL_INCLUDE_DIRECTORIES})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11)
